#include "mainwindow.hpp"
#include <QApplication>
#include <QKeyEvent>
#include <QMessageBox>
#include <QGridLayout>
#include <QFormLayout>
#include <QScrollArea>
#include <QSpinBox>
#include <QDoubleSpinBox>

void MainWindow::setupUI()
{
    centralWidget_ = new QWidget(this);
    centralWidget_->setContentsMargins(0, 0, 0, 0); // Remove margins from central widget
    setCentralWidget(centralWidget_);
    
    mainLayout_ = new QVBoxLayout(centralWidget_);
    mainLayout_->setContentsMargins(0, 0, 0, 0); // Remove all margins from main layout
    mainLayout_->setSpacing(0); // Remove spacing
    
    // Create main tab widget
    mainTabWidget_ = new QTabWidget(this);
    mainTabWidget_->setContentsMargins(0, 0, 0, 0); // Remove margins from tab widget
    mainLayout_->addWidget(mainTabWidget_);
    
    // Create Synthesizer tab
    QWidget *synthesizerTab = new QWidget();
    QVBoxLayout *synthesizerLayout = new QVBoxLayout(synthesizerTab);
    synthesizerLayout->setSpacing(0); // Remove spacing between elements
    synthesizerLayout->setContentsMargins(0, 0, 0, 0); // Remove margins
    
    // Create keyboard widget (now takes full width)
    keyboardWidget_ = new KeyboardWidget(this);
    keyboardWidget_->setMinimumHeight(100);
    keyboardWidget_->setMinimumWidth(200); // Set a reasonable minimum width for 4 octaves
    keyboardWidget_->setAlignment(Qt::AlignLeft); // Align keyboard to the left to remove padding
    keyboardWidget_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed); // Allow horizontal expansion
    keyboardWidget_->setContentsMargins(0, 0, 0, 0); // Remove padding around keyboard
    keyboardWidget_->setMouseTracking(false); // Enable mouse events for clicking
    
    synthesizerLayout->addWidget(keyboardWidget_);
    
    // Create controls layout for left controls (now at bottom)
    controlsLayout_ = new QHBoxLayout();
    
    // Pitch bend control
    QGroupBox *pitchBendGroup = new QGroupBox("Pitch Bend", synthesizerTab);
    QVBoxLayout *pitchBendLayout = new QVBoxLayout(pitchBendGroup);
    pitchBendGroup->setMinimumHeight(100); // Set minimum height only
    
    pitchBendSlider_ = new QSlider(Qt::Vertical, synthesizerTab);
    pitchBendSlider_->setRange(-200, 200); // -2 to +2 semitones
    pitchBendSlider_->setValue(0);
    pitchBendSlider_->setTickPosition(QSlider::TicksRight);
    pitchBendSlider_->setTickInterval(50);
    connect(pitchBendSlider_, &QSlider::valueChanged, this, &MainWindow::onPitchBendChanged);
    
    pitchBendLabel_ = new QLabel("0", synthesizerTab);
    pitchBendLabel_->setAlignment(Qt::AlignCenter);
    
    pitchBendLayout->addWidget(pitchBendSlider_);
    pitchBendLayout->addWidget(pitchBendLabel_);
    controlsLayout_->addWidget(pitchBendGroup);
    
    // Modulation wheel control
    QGroupBox *modWheelGroup = new QGroupBox("Mod Wheel", synthesizerTab);
    QVBoxLayout *modWheelLayout = new QVBoxLayout(modWheelGroup);
    modWheelGroup->setMinimumHeight(100); // Set minimum height only
    
    modWheelSlider_ = new QSlider(Qt::Vertical, synthesizerTab);
    modWheelSlider_->setRange(0, 127);
    modWheelSlider_->setValue(0);
    modWheelSlider_->setTickPosition(QSlider::TicksRight);
    modWheelSlider_->setTickInterval(20);
    connect(modWheelSlider_, &QSlider::valueChanged, this, &MainWindow::onModWheelChanged);
    
    modWheelLabel_ = new QLabel("0", synthesizerTab);
    modWheelLabel_->setAlignment(Qt::AlignCenter);
    
    modWheelLayout->addWidget(modWheelSlider_);
    modWheelLayout->addWidget(modWheelLabel_);
    controlsLayout_->addWidget(modWheelGroup);
    
    // Volume control
    volumeGroup_ = new QGroupBox("Volume", synthesizerTab);
    QVBoxLayout *volumeLayout = new QVBoxLayout(volumeGroup_);
    volumeGroup_->setMinimumHeight(100); // Set minimum height only
    
    volumeSlider_ = new QSlider(Qt::Vertical, synthesizerTab);
    volumeSlider_->setRange(0, 100);
    volumeSlider_->setValue(50);
    volumeSlider_->setTickPosition(QSlider::TicksRight);
    volumeSlider_->setTickInterval(10);
    connect(volumeSlider_, &QSlider::valueChanged, this, &MainWindow::onVolumeChanged);
    
    volumeLabel_ = new QLabel("50%", synthesizerTab);
    volumeLabel_->setAlignment(Qt::AlignCenter);
    
    volumeLayout->addWidget(volumeSlider_);
    volumeLayout->addWidget(volumeLabel_);
    controlsLayout_->addWidget(volumeGroup_);
    
    // Octave selector
    QGroupBox *octaveGroup = new QGroupBox("Octave", synthesizerTab);
    QVBoxLayout *octaveLayout = new QVBoxLayout(octaveGroup);
    octaveGroup->setMinimumHeight(100); // Set minimum height only
    
    octaveSpinBox_ = new QSpinBox(synthesizerTab);
    octaveSpinBox_->setRange(2, 4); // C2 to C4 (since we show 4 octaves, max is C4 to show C4-C7)
    octaveSpinBox_->setValue(2); // Start at C2 to show C2-C5
    octaveSpinBox_->setSuffix(" (C)");
    connect(octaveSpinBox_, QOverload<int>::of(&QSpinBox::valueChanged), this, &MainWindow::onOctaveChanged);
    
    octaveLabel_ = new QLabel("C2", synthesizerTab);
    octaveLabel_->setAlignment(Qt::AlignCenter);
    octaveLabel_->setStyleSheet("font-weight: bold; font-size: 12px;");
    
    octaveLayout->addWidget(octaveSpinBox_);
    octaveLayout->addWidget(octaveLabel_);
    controlsLayout_->addWidget(octaveGroup);
    
    synthesizerLayout->addLayout(controlsLayout_);
    
    // Create a new tab widget for the other controls
    QTabWidget *controlsTabWidget = new QTabWidget(synthesizerTab);
    
    // Create Controls tab
    QWidget *controlsTab = new QWidget();
    QVBoxLayout *controlsTabLayout = new QVBoxLayout(controlsTab);
    
    // Create horizontal layout for the controls
    QHBoxLayout *controlsHorizontalLayout = new QHBoxLayout();
    
    // Preset selection
    presetGroup_ = new QGroupBox("Preset", controlsTab);
    QVBoxLayout *presetLayout = new QVBoxLayout(presetGroup_);
    
    presetCombo_ = new QComboBox(controlsTab);
    
    // Add all presets from the preset manager
    auto presetNames = presetManager_->getPresetNames();
    for (const auto& presetName : presetNames) {
        presetCombo_->addItem(QString::fromStdString(presetName));
    }
    
    connect(presetCombo_, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &MainWindow::onPresetChanged);
    
    presetLayout->addWidget(presetCombo_);
    controlsHorizontalLayout->addWidget(presetGroup_);
    
    // Effects controls
    effectsGroup_ = new QGroupBox("Effects", controlsTab);
    QGridLayout *effectsLayout = new QGridLayout(effectsGroup_);
    
    // Reverb
    reverbSlider_ = new QSlider(Qt::Horizontal, controlsTab);
    reverbSlider_->setRange(0, 100);
    reverbSlider_->setValue(0);
    reverbSlider_->setTickPosition(QSlider::TicksBelow);
    reverbSlider_->setTickInterval(10);
    connect(reverbSlider_, &QSlider::valueChanged, this, &MainWindow::onReverbChanged);
    
    reverbLabel_ = new QLabel("0%", controlsTab);
    reverbLabel_->setAlignment(Qt::AlignCenter);
    
    effectsLayout->addWidget(new QLabel("Reverb:", controlsTab), 0, 0);
    effectsLayout->addWidget(reverbSlider_, 0, 1);
    effectsLayout->addWidget(reverbLabel_, 0, 2);
    
    // Chorus
    chorusSlider_ = new QSlider(Qt::Horizontal, controlsTab);
    chorusSlider_->setRange(0, 100);
    chorusSlider_->setValue(0);
    chorusSlider_->setTickPosition(QSlider::TicksBelow);
    chorusSlider_->setTickInterval(10);
    connect(chorusSlider_, &QSlider::valueChanged, this, &MainWindow::onChorusChanged);
    
    chorusLabel_ = new QLabel("0%", controlsTab);
    chorusLabel_->setAlignment(Qt::AlignCenter);
    
    effectsLayout->addWidget(new QLabel("Chorus:", controlsTab), 1, 0);
    effectsLayout->addWidget(chorusSlider_, 1, 1);
    effectsLayout->addWidget(chorusLabel_, 1, 2);
    
    // Distortion
    distortionSlider_ = new QSlider(Qt::Horizontal, controlsTab);
    distortionSlider_->setRange(0, 100);
    distortionSlider_->setValue(0);
    distortionSlider_->setTickPosition(QSlider::TicksBelow);
    distortionSlider_->setTickInterval(10);
    connect(distortionSlider_, &QSlider::valueChanged, this, &MainWindow::onDistortionChanged);
    
    distortionLabel_ = new QLabel("0%", controlsTab);
    distortionLabel_->setAlignment(Qt::AlignCenter);
    
    effectsLayout->addWidget(new QLabel("Distortion:", controlsTab), 2, 0);
    effectsLayout->addWidget(distortionSlider_, 2, 1);
    effectsLayout->addWidget(distortionLabel_, 2, 2);
    
    controlsHorizontalLayout->addWidget(effectsGroup_);
    
    // Algorithm selection
    algorithmGroup_ = new QGroupBox("Algorithm", controlsTab);
    QVBoxLayout *algorithmLayout = new QVBoxLayout(algorithmGroup_);
    
    algorithmCombo_ = new QComboBox(controlsTab);
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
    controlsHorizontalLayout->addWidget(algorithmGroup_);
    
    // Channel selection
    channelGroup_ = new QGroupBox("Channel", controlsTab);
    QVBoxLayout *channelLayout = new QVBoxLayout(channelGroup_);
    
    channelCombo_ = new QComboBox(controlsTab);
    for (int i = 0; i < 8; i++) {
        channelCombo_->addItem(QString("Channel %1").arg(i + 1));
    }
    
    connect(channelCombo_, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &MainWindow::onChannelChanged);
    
    channelLayout->addWidget(channelCombo_);
    controlsHorizontalLayout->addWidget(channelGroup_);
    
    // Synthesizer management
    synthManagerGroup_ = new QGroupBox("Synthesizers", controlsTab);
    QVBoxLayout *synthManagerLayout = new QVBoxLayout(synthManagerGroup_);
    
    synthSelector_ = new QComboBox(controlsTab);
    connect(synthSelector_, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &MainWindow::onSynthesizerChanged);
    
    QHBoxLayout *synthButtonLayout = new QHBoxLayout();
    addSynthButton_ = new QPushButton("Add Synth", controlsTab);
    removeSynthButton_ = new QPushButton("Remove Synth", controlsTab);
    
    connect(addSynthButton_, &QPushButton::clicked, this, &MainWindow::onAddSynthesizer);
    connect(removeSynthButton_, &QPushButton::clicked, this, &MainWindow::onRemoveSynthesizer);
    
    synthButtonLayout->addWidget(addSynthButton_);
    synthButtonLayout->addWidget(removeSynthButton_);
    
    synthManagerLayout->addWidget(synthSelector_);
    synthManagerLayout->addLayout(synthButtonLayout);
    controlsHorizontalLayout->addWidget(synthManagerGroup_);
    
    // Add the horizontal layout to the vertical tab layout
    controlsTabLayout->addLayout(controlsHorizontalLayout);
    
    // Add the controls tab to the tab widget
    controlsTabWidget->addTab(controlsTab, "Controls");
    
    synthesizerLayout->addWidget(controlsTabWidget);
    
    // Create advanced controls tab widget
    QTabWidget *advancedTabWidget = new QTabWidget(synthesizerTab);
    
    // Operators tab
    QWidget *operatorsTab = new QWidget();
    QVBoxLayout *operatorsLayout = new QVBoxLayout(operatorsTab);
    
    // Add info section
    QGroupBox *infoGroup = new QGroupBox("Info", operatorsTab);
    QVBoxLayout *infoLayout = new QVBoxLayout(infoGroup);
    
    QLabel *infoLabel = new QLabel(
        "FM Synthesizer with 6 Operators\n\n"
        "Each note gets its own voice with 6 operators\n"
        "Operator settings are configured through presets\n"
        "32 different algorithms control operator routing\n"
        "Real-time modulation via pitch bend and mod wheel\n\n"
        "Basic Controls:\n"
        "Max Voices: 16\n"
        "Sample Rate: 44.1 kHz\n"
        "Audio Format: 14-bit", operatorsTab);
    infoLabel->setWordWrap(true);
    infoLayout->addWidget(infoLabel);
    operatorsLayout->addWidget(infoGroup);
    
    advancedTabWidget->addTab(operatorsTab, "Operators");
    
    // Modulation tab
    QWidget *modulationTab = new QWidget();
    QVBoxLayout *modulationLayout = new QVBoxLayout(modulationTab);
    
    // Pitch bend and mod wheel controls moved to main synthesizer tab
    
    advancedTabWidget->addTab(modulationTab, "Modulation");
    
    // Internals tab
    QWidget *internalsTab = new QWidget();
    QVBoxLayout *internalsLayout = new QVBoxLayout(internalsTab);
    
    // Create scroll area for internals
    QScrollArea *scrollArea = new QScrollArea(internalsTab);
    QWidget *scrollContent = new QWidget();
    QVBoxLayout *scrollLayout = new QVBoxLayout(scrollContent);
    
    // Audio parameters
    QGroupBox *audioGroup = new QGroupBox("Audio Parameters", scrollContent);
    QFormLayout *audioLayout = new QFormLayout(audioGroup);
    audioLayout->setFieldGrowthPolicy(QFormLayout::FieldsStayAtSizeHint);
    audioLayout->setFormAlignment(Qt::AlignLeft);
    audioLayout->setLabelAlignment(Qt::AlignLeft);
    
    // Get current synthesizer for default values
    auto* currentSynth = getCurrentSynthesizer();
    
    // Audio Bits
    audioBitsSpinBox_ = new QSpinBox(scrollContent);
    audioBitsSpinBox_->setRange(8, 24);
    audioBitsSpinBox_->setValue(currentSynth ? currentSynth->getAudioBits() : toybasic::Constants::AUDIO_BITS);
    audioLayout->addRow("Audio Bits:", audioBitsSpinBox_);
    
    // Audio Max Value
    audioMaxSpinBox_ = new QSpinBox(scrollContent);
    audioMaxSpinBox_->setRange(127, 16777215);
    audioMaxSpinBox_->setValue(currentSynth ? currentSynth->getAudioMaxValue() : toybasic::Constants::AUDIO_MAX_VALUE);
    audioLayout->addRow("Audio Max Value:", audioMaxSpinBox_);
    
    // Audio Min Value
    audioMinSpinBox_ = new QSpinBox(scrollContent);
    audioMinSpinBox_->setRange(-16777216, -127);
    audioMinSpinBox_->setValue(currentSynth ? currentSynth->getAudioMinValue() : toybasic::Constants::AUDIO_MIN_VALUE);
    audioLayout->addRow("Audio Min Value:", audioMinSpinBox_);
    
    // Audio Scale
    audioScaleSpinBox_ = new QDoubleSpinBox(scrollContent);
    audioScaleSpinBox_->setRange(1.0, 1000000.0);
    audioScaleSpinBox_->setValue(currentSynth ? currentSynth->getAudioScale() : toybasic::Constants::AUDIO_SCALE);
    audioScaleSpinBox_->setDecimals(1);
    audioLayout->addRow("Audio Scale:", audioScaleSpinBox_);
    
    scrollLayout->addWidget(audioGroup);
    
    // MIDI parameters
    QGroupBox *midiGroup = new QGroupBox("MIDI Parameters", scrollContent);
    QFormLayout *midiLayout = new QFormLayout(midiGroup);
    midiLayout->setFieldGrowthPolicy(QFormLayout::FieldsStayAtSizeHint);
    midiLayout->setFormAlignment(Qt::AlignLeft);
    midiLayout->setLabelAlignment(Qt::AlignLeft);
    
    // MIDI A4 Note
    midiA4NoteSpinBox_ = new QSpinBox(scrollContent);
    midiA4NoteSpinBox_->setRange(0, 127);
    midiA4NoteSpinBox_->setValue(currentSynth ? currentSynth->getMidiA4Note() : toybasic::Constants::MIDI_A4_NOTE);
    midiLayout->addRow("MIDI A4 Note:", midiA4NoteSpinBox_);
    
    // MIDI A4 Frequency
    midiA4FreqSpinBox_ = new QDoubleSpinBox(scrollContent);
    midiA4FreqSpinBox_->setRange(200.0, 1000.0);
    midiA4FreqSpinBox_->setValue(currentSynth ? currentSynth->getMidiA4Frequency() : toybasic::Constants::MIDI_A4_FREQUENCY);
    midiA4FreqSpinBox_->setDecimals(1);
    midiLayout->addRow("MIDI A4 Frequency:", midiA4FreqSpinBox_);
    
    // MIDI Notes Per Octave
    midiNotesSpinBox_ = new QSpinBox(scrollContent);
    midiNotesSpinBox_->setRange(8, 24);
    midiNotesSpinBox_->setValue(currentSynth ? currentSynth->getMidiNotesPerOctave() : toybasic::Constants::MIDI_NOTES_PER_OCTAVE);
    midiLayout->addRow("MIDI Notes Per Octave:", midiNotesSpinBox_);
    
    scrollLayout->addWidget(midiGroup);
    
    // Synthesizer limits
    QGroupBox *limitsGroup = new QGroupBox("Synthesizer Limits", scrollContent);
    QFormLayout *limitsLayout = new QFormLayout(limitsGroup);
    limitsLayout->setFieldGrowthPolicy(QFormLayout::FieldsStayAtSizeHint);
    limitsLayout->setFormAlignment(Qt::AlignLeft);
    limitsLayout->setLabelAlignment(Qt::AlignLeft);
    
    // Max Voices
    maxVoicesSpinBox_ = new QSpinBox(scrollContent);
    maxVoicesSpinBox_->setRange(1, 64);
    maxVoicesSpinBox_->setValue(currentSynth ? currentSynth->getMaxVoices() : toybasic::Constants::MAX_VOICES);
    limitsLayout->addRow("Max Voices:", maxVoicesSpinBox_);
    
    // Max Operators
    maxOpsSpinBox_ = new QSpinBox(scrollContent);
    maxOpsSpinBox_->setRange(1, 12);
    maxOpsSpinBox_->setValue(currentSynth ? currentSynth->getMaxOperators() : toybasic::Constants::MAX_OPERATORS);
    limitsLayout->addRow("Max Operators:", maxOpsSpinBox_);
    
    // Max Channels
    maxChannelsSpinBox_ = new QSpinBox(scrollContent);
    maxChannelsSpinBox_->setRange(1, 32);
    maxChannelsSpinBox_->setValue(currentSynth ? currentSynth->getMaxChannels() : toybasic::Constants::MAX_CHANNELS);
    limitsLayout->addRow("Max Channels:", maxChannelsSpinBox_);
    
    // Max Algorithms
    maxAlgsSpinBox_ = new QSpinBox(scrollContent);
    maxAlgsSpinBox_->setRange(1, 64);
    maxAlgsSpinBox_->setValue(currentSynth ? currentSynth->getMaxAlgorithms() : toybasic::Constants::MAX_ALGORITHMS);
    limitsLayout->addRow("Max Algorithms:", maxAlgsSpinBox_);
    
    scrollLayout->addWidget(limitsGroup);
    
    // Envelope timing limits
    QGroupBox *envelopeGroup = new QGroupBox("Envelope Timing Limits", scrollContent);
    QFormLayout *envelopeLayout = new QFormLayout(envelopeGroup);
    envelopeLayout->setFieldGrowthPolicy(QFormLayout::FieldsStayAtSizeHint);
    envelopeLayout->setFormAlignment(Qt::AlignLeft);
    envelopeLayout->setLabelAlignment(Qt::AlignLeft);
    
    // Min Envelope Time
    minEnvTimeSpinBox_ = new QDoubleSpinBox(scrollContent);
    minEnvTimeSpinBox_->setRange(0.001, 1.0);
    minEnvTimeSpinBox_->setValue(currentSynth ? currentSynth->getMinEnvelopeTime() : toybasic::Constants::MIN_ENVELOPE_TIME);
    minEnvTimeSpinBox_->setDecimals(3);
    envelopeLayout->addRow("Min Envelope Time:", minEnvTimeSpinBox_);
    
    // Max Envelope Time
    maxEnvTimeSpinBox_ = new QDoubleSpinBox(scrollContent);
    maxEnvTimeSpinBox_->setRange(1.0, 30.0);
    maxEnvTimeSpinBox_->setValue(currentSynth ? currentSynth->getMaxEnvelopeTime() : toybasic::Constants::MAX_ENVELOPE_TIME);
    maxEnvTimeSpinBox_->setDecimals(1);
    envelopeLayout->addRow("Max Envelope Time:", maxEnvTimeSpinBox_);
    
    scrollLayout->addWidget(envelopeGroup);
    
    // Volume and amplitude limits
    QGroupBox *volumeGroup = new QGroupBox("Volume & Amplitude Limits", scrollContent);
    QFormLayout *volumeLimitsLayout = new QFormLayout(volumeGroup);
    volumeLimitsLayout->setFieldGrowthPolicy(QFormLayout::FieldsStayAtSizeHint);
    volumeLimitsLayout->setFormAlignment(Qt::AlignLeft);
    volumeLimitsLayout->setLabelAlignment(Qt::AlignLeft);
    
    // Min Volume
    minVolumeSpinBox_ = new QDoubleSpinBox(scrollContent);
    minVolumeSpinBox_->setRange(0.0, 1.0);
    minVolumeSpinBox_->setValue(currentSynth ? currentSynth->getMinVolume() : toybasic::Constants::MIN_VOLUME);
    minVolumeSpinBox_->setDecimals(2);
    volumeLimitsLayout->addRow("Min Volume:", minVolumeSpinBox_);
    
    // Max Volume
    maxVolumeSpinBox_ = new QDoubleSpinBox(scrollContent);
    maxVolumeSpinBox_->setRange(0.0, 2.0);
    maxVolumeSpinBox_->setValue(currentSynth ? currentSynth->getMaxVolume() : toybasic::Constants::MAX_VOLUME);
    maxVolumeSpinBox_->setDecimals(2);
    volumeLimitsLayout->addRow("Max Volume:", maxVolumeSpinBox_);
    
    // Min Amplitude
    minAmplitudeSpinBox_ = new QDoubleSpinBox(scrollContent);
    minAmplitudeSpinBox_->setRange(0.0, 1.0);
    minAmplitudeSpinBox_->setValue(currentSynth ? currentSynth->getMinAmplitude() : toybasic::Constants::MIN_AMPLITUDE);
    minAmplitudeSpinBox_->setDecimals(2);
    volumeLimitsLayout->addRow("Min Amplitude:", minAmplitudeSpinBox_);
    
    // Max Amplitude
    maxAmplitudeSpinBox_ = new QDoubleSpinBox(scrollContent);
    maxAmplitudeSpinBox_->setRange(0.0, 2.0);
    maxAmplitudeSpinBox_->setValue(currentSynth ? currentSynth->getMaxAmplitude() : toybasic::Constants::MAX_AMPLITUDE);
    maxAmplitudeSpinBox_->setDecimals(2);
    volumeLimitsLayout->addRow("Max Amplitude:", maxAmplitudeSpinBox_);
    
    scrollLayout->addWidget(volumeGroup);
    
    scrollArea->setWidget(scrollContent);
    scrollArea->setWidgetResizable(true);
    internalsLayout->addWidget(scrollArea);
    
    // Setup connections for internals tab controls
    setupInternalsTabConnections();
    
    advancedTabWidget->addTab(internalsTab, "Internals");
    
    synthesizerLayout->addWidget(advancedTabWidget);
    
    // Add Synthesizer tab to main tab widget
    mainTabWidget_->addTab(synthesizerTab, "Synthesizer");
    
    // Create Tracker tab
    trackerWidget_ = new TrackerWidget();
    
    // Connect tracker signals to synthesizer
    connect(trackerWidget_, &TrackerWidget::noteTriggered, this, &MainWindow::onTrackerNoteTriggered);
    connect(trackerWidget_, &TrackerWidget::noteReleased, this, &MainWindow::onTrackerNoteReleased);
    
    // Add Tracker tab to main tab widget
    mainTabWidget_->addTab(trackerWidget_, "Tracker");
    
    // Initialize synthesizer selector after all UI elements are created
    updateSynthesizerSelector();
    
    // Set window properties
    setWindowTitle("FM Synthesizer - Advanced 4-Operator FM Synthesizer");
    setMinimumSize(800, 600); // Reduced minimum size to allow better resizing
    resize(1400, 900);
    setFocusPolicy(Qt::StrongFocus); // Ensure we can receive key events
}
