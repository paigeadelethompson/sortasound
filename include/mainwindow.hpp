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
#include <memory>
#include <map>
#include <set>
#include <vector>

#include "presets.hpp"
#include "keyboardwidget.hpp"
#include "fm.hpp"

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

private:
    void setupUI();
    void setupKeyboardMapping();
    void setupOperatorTabs();
    int keyToNote(Qt::Key key) const;
    void noteOn(int note);
    void noteOff(int note);
    void allNotesOff();

    // UI Components
    QWidget *centralWidget_;
    QVBoxLayout *mainLayout_;
    QHBoxLayout *controlsLayout_;
    
    
    // FM Synthesizer
    std::unique_ptr<toybasic::FMSynthesizer> synthesizer_;
    
    // Preset Manager
    std::unique_ptr<toybasic::PresetManager> presetManager_;
    
    // Keyboard Widget
    KeyboardWidget *keyboardWidget_;
    
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
    
    QGroupBox *operatorGroup_;
    QTabWidget *operatorTabs_;
    std::vector<QWidget*> operatorPages_;
    
    QGroupBox *modulationGroup_;
    QSlider *pitchBendSlider_;
    QSlider *modWheelSlider_;
    QLabel *pitchBendLabel_;
    QLabel *modWheelLabel_;
    
    QGroupBox *channelGroup_;
    QComboBox *channelCombo_;
    
    // Keyboard mapping
    std::map<Qt::Key, int> keyToNoteMap_;
    std::set<int> activeNotes_;
    int currentChannel_;
    
    // Constants for keyboard layout
    static constexpr int OCTAVE_START = 60; // Middle C (C4)
    static constexpr int NOTES_PER_OCTAVE = 12;
};
