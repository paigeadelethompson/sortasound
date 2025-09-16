#pragma once

#include <QMainWindow>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QSlider>
#include <QLabel>
#include <QComboBox>
#include <QGroupBox>
#include <QKeyEvent>
#include <QTimer>
#include <QTabWidget>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <memory>
#include <map>
#include <set>
#include <vector>

#include "presets.hpp"
#include "keyboardwidget.hpp"
#include "fm.hpp"
#include "tracker.hpp"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;
    void focusInEvent(QFocusEvent *event) override;
    void focusOutEvent(QFocusEvent *event) override;

private slots:
    void onVolumeChanged(int value);
    void onPresetChanged(int index);
    void onReverbChanged(int value);
    void onChorusChanged(int value);
    void onDistortionChanged(int value);
    void onAlgorithmChanged(int index);
    void onChannelChanged(int index);
    void onPitchBendChanged(int value);
    void onModWheelChanged(int value);
    void onOperatorParameterChanged();
    void onSynthesizerChanged(int index);
    void onAddSynthesizer();
    void onRemoveSynthesizer();
    void refreshInternalsTab();
    void onTrackerNoteTriggered(int note, int velocity, int channel);
    void onTrackerNoteReleased(int note, int channel);
    void onOctaveChanged(int octave);
    void onKeyboardKeyPressed(int note);
    void onKeyboardKeyReleased(int note);

private:
    void setupUI();
    void setupKeyboardMapping();
    void updateKeyboardMapping();
    void setupInternalsTabConnections();
    int keyToNote(Qt::Key key) const;
    void noteOn(int note);
    void noteOff(int note);
    void allNotesOff();
    void updateSynthesizerSelector();
    toybasic::FMSynthesizer* getCurrentSynthesizer();

    // UI Components
    QWidget *centralWidget_;
    QVBoxLayout *mainLayout_;
    QHBoxLayout *controlsLayout_;
    QTabWidget *mainTabWidget_;
    
    
    // FM Synthesizer instances
    std::vector<std::unique_ptr<toybasic::FMSynthesizer>> synthesizers_;
    int currentSynthesizerIndex_;
    
    // Preset Manager
    std::unique_ptr<toybasic::PresetManager> presetManager_;
    
    // Keyboard Widget
    KeyboardWidget *keyboardWidget_;
    
    // Tracker Widget
    TrackerWidget *trackerWidget_;
    
    // Octave selector
    QGroupBox *octaveGroup_;
    QSpinBox *octaveSpinBox_;
    QLabel *octaveLabel_;
    
    // Control Widgets
    QGroupBox *volumeGroup_;
    QSlider *volumeSlider_;
    QLabel *volumeLabel_;
    
    QGroupBox *presetGroup_;
    QComboBox *presetCombo_;
    
    QGroupBox *effectsGroup_;
    QSlider *reverbSlider_;
    QLabel *reverbLabel_;
    QSlider *chorusSlider_;
    QLabel *chorusLabel_;
    QSlider *distortionSlider_;
    QLabel *distortionLabel_;
    
    // Advanced controls
    QGroupBox *algorithmGroup_;
    QComboBox *algorithmCombo_;
    
    // Internals tab controls
    QSpinBox *audioBitsSpinBox_;
    QSpinBox *audioMaxSpinBox_;
    QSpinBox *audioMinSpinBox_;
    QDoubleSpinBox *audioScaleSpinBox_;
    QSpinBox *midiA4NoteSpinBox_;
    QDoubleSpinBox *midiA4FreqSpinBox_;
    QSpinBox *midiNotesSpinBox_;
    QSpinBox *maxVoicesSpinBox_;
    QSpinBox *maxOpsSpinBox_;
    QSpinBox *maxChannelsSpinBox_;
    QSpinBox *maxAlgsSpinBox_;
    QDoubleSpinBox *minEnvTimeSpinBox_;
    QDoubleSpinBox *maxEnvTimeSpinBox_;
    QDoubleSpinBox *minVolumeSpinBox_;
    QDoubleSpinBox *maxVolumeSpinBox_;
    QDoubleSpinBox *minAmplitudeSpinBox_;
    QDoubleSpinBox *maxAmplitudeSpinBox_;
    
    // Old operator and modulation groups removed - now using tabs
    QSlider *pitchBendSlider_;
    QSlider *modWheelSlider_;
    QLabel *pitchBendLabel_;
    QLabel *modWheelLabel_;
    
    QGroupBox *channelGroup_;
    QComboBox *channelCombo_;
    
    // Synthesizer management
    QGroupBox *synthManagerGroup_;
    QComboBox *synthSelector_;
    QPushButton *addSynthButton_;
    QPushButton *removeSynthButton_;
    
    // Keyboard mapping
    std::map<Qt::Key, int> keyToNoteMap_;
    std::set<int> activeNotes_;
    int currentChannel_;
    
    // Constants for keyboard layout
    static constexpr int OCTAVE_START = 60; // Middle C (C4)
    static constexpr int NOTES_PER_OCTAVE = 12;
};
