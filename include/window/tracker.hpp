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

#include <QWidget>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QSpinBox>
#include <QLabel>
#include <QGroupBox>
#include <QComboBox>
#include <QSlider>
#include <QTimer>
#include <QCheckBox>
#include <vector>
#include <map>
#include <memory>

class TrackerPattern;
class TrackerStep;
class TrackerChannel;
class TrackerStep {
public:
    TrackerStep();
    
    int note;
    
    int instrument;
    
    int volume;
    
    int effect1;
    int effect2;
    int effect3;
    
    bool active;
    
    QString toString() const;
    
    void fromString(const QString& str);
};

class TrackerChannel {
public:
    TrackerChannel(int steps = 64);
    
    TrackerStep& getStep(int position);
    const TrackerStep& getStep(int position) const;
    void setStep(int position, const TrackerStep& step);
    
    int getSteps() const { return stepCount_; }
    void setSteps(int steps);
    
    QString getName() const { return name_; }
    void setName(const QString& name) { name_ = name; }
    
    bool isMuted() const { return muted_; }
    void setMuted(bool muted) { muted_ = muted; }
    
    bool isSolo() const { return solo_; }
    void setSolo(bool solo) { solo_ = solo; }
    
private:
    std::vector<TrackerStep> steps_;
    int stepCount_;
    QString name_;
    bool muted_;
    bool solo_;
};

class TrackerPattern {
public:
    TrackerPattern(int channels = 8, int steps = 64);
    
    TrackerChannel& getChannel(int channel);
    const TrackerChannel& getChannel(int channel) const;
    
    int getChannels() const { return channelCount_; }
    int getSteps() const { return stepCount_; }
    
    QString getName() const { return name_; }
    void setName(const QString& name) { name_ = name; }
    
    void clear();
    
private:
    std::vector<std::unique_ptr<TrackerChannel>> channels_;
    int channelCount_;
    int stepCount_;
    QString name_;
};

class TrackerWidget : public QWidget {
    Q_OBJECT

public:
    explicit TrackerWidget(QWidget *parent = nullptr);
    ~TrackerWidget();
    
    void addPattern();
    void removePattern(int index);
    void duplicatePattern(int index);
    void setCurrentPattern(int index);
    int getCurrentPattern() const { return currentPattern_; }
    
    void play();
    void stop();
    void pause();
    bool isPlaying() const { return playing_; }
    
    void setTempo(int bpm);
    int getTempo() const { return tempo_; }
    
    void setPatternSteps(int steps);
    void setPatternChannels(int channels);
    
signals:
    void noteTriggered(int note, int velocity, int channel);
    void noteReleased(int note, int channel);
    void tempoChanged(int bpm);

private slots:
    void onPlayClicked();
    void onStopClicked();
    void onPauseClicked();
    void onTempoChanged(int value);
    void onPatternChanged(int index);
    void onStepChanged(int row, int column);
    void onChannelMuteToggled(int channel, bool muted);
    void onChannelSoloToggled(int channel, bool solo);
    void onPlaybackTimer();

private:
    void setupUI();
    void setupPatternTable();
    void updatePatternTable();
    void updatePlaybackPosition();
    void triggerStep(int step);
    void clearPlaybackPosition();
    
    QVBoxLayout *mainLayout_;
    QHBoxLayout *controlsLayout_;
    QHBoxLayout *patternControlsLayout_;
    
    QPushButton *playButton_;
    QPushButton *stopButton_;
    QPushButton *pauseButton_;
    QLabel *tempoLabel_;
    QSlider *tempoSlider_;
    QSpinBox *tempoSpinBox_;
    
    QLabel *patternLabel_;
    QComboBox *patternCombo_;
    QPushButton *addPatternButton_;
    QPushButton *removePatternButton_;
    QPushButton *duplicatePatternButton_;
    
    QTableWidget *patternTable_;
    
    QGroupBox *channelControlsGroup_;
    std::vector<QCheckBox*> channelMuteCheckboxes_;
    std::vector<QCheckBox*> channelSoloCheckboxes_;
    
    std::vector<std::unique_ptr<TrackerPattern>> patterns_;
    int currentPattern_;
    int currentStep_;
    bool playing_;
    bool paused_;
    int tempo_;
    
    QTimer *playbackTimer_;
    
    static constexpr int DEFAULT_TEMPO = 120;
    static constexpr int DEFAULT_CHANNELS = 8;
    static constexpr int DEFAULT_STEPS = 64;
    static constexpr int MIN_TEMPO = 60;
    static constexpr int MAX_TEMPO = 200;
};
