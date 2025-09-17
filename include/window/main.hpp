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

#include "../fm/presets.hpp"
#include "../widget/keyboard.hpp"
#include "../widget/operator.hpp"
#include "../fm/fm.hpp"

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
    void refreshInternalsTab();
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
    toybasic::FMSynthesizer* getCurrentSynthesizer();

    QWidget *centralWidget_;
    QVBoxLayout *mainLayout_;
    QHBoxLayout *controlsLayout_;
    
    
    std::vector<std::unique_ptr<toybasic::FMSynthesizer>> synthesizers_;
    int currentSynthesizerIndex_;
    
    std::unique_ptr<toybasic::PresetManager> presetManager_;
    
    KeyboardWidget *keyboardWidget_;
    OperatorGraphWidget *operatorGraphWidget_;
    
    QGroupBox *octaveGroup_;
    QSpinBox *octaveSpinBox_;
    QLabel *octaveLabel_;
    
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
    
    QGroupBox *algorithmGroup_;
    QComboBox *algorithmCombo_;
    
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
    
    QSlider *pitchBendSlider_;
    QSlider *modWheelSlider_;
    QLabel *pitchBendLabel_;
    QLabel *modWheelLabel_;
    
    QGroupBox *channelGroup_;
    QComboBox *channelCombo_;
    
    QTimer *pitchBendReturnTimer_;
    
    std::map<Qt::Key, int> keyToNoteMap_;
    std::set<int> activeNotes_;
    int currentChannel_;
    
    static constexpr int OCTAVE_START = 60;
    static constexpr int NOTES_PER_OCTAVE = 12;
};
