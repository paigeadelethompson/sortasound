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

// Represents a single step in a tracker pattern
class TrackerStep {
public:
    TrackerStep();
    
    // Note data (0-127, 0 = no note)
    int note;
    
    // Instrument/patch number
    int instrument;
    
    // Volume (0-127)
    int volume;
    
    // Effects
    int effect1;
    int effect2;
    int effect3;
    
    // Whether this step is active
    bool active;
    
    // Convert to string for display
    QString toString() const;
    
    // Parse from string
    void fromString(const QString& str);
};

// Represents a channel in a tracker pattern
class TrackerChannel {
public:
    TrackerChannel(int steps = 64);
    
    // Get/set step at position
    TrackerStep& getStep(int position);
    const TrackerStep& getStep(int position) const;
    void setStep(int position, const TrackerStep& step);
    
    // Channel properties
    int getSteps() const { return stepCount_; }
    void setSteps(int steps);
    
    // Channel name
    QString getName() const { return name_; }
    void setName(const QString& name) { name_ = name; }
    
    // Mute/solo
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

// Represents a complete pattern
class TrackerPattern {
public:
    TrackerPattern(int channels = 8, int steps = 64);
    
    // Get/set channel
    TrackerChannel& getChannel(int channel);
    const TrackerChannel& getChannel(int channel) const;
    
    // Pattern properties
    int getChannels() const { return channelCount_; }
    int getSteps() const { return stepCount_; }
    
    // Pattern name
    QString getName() const { return name_; }
    void setName(const QString& name) { name_ = name; }
    
    // Clear pattern
    void clear();
    
private:
    std::vector<std::unique_ptr<TrackerChannel>> channels_;
    int channelCount_;
    int stepCount_;
    QString name_;
};

// Main tracker widget
class TrackerWidget : public QWidget {
    Q_OBJECT

public:
    explicit TrackerWidget(QWidget *parent = nullptr);
    ~TrackerWidget();
    
    // Pattern management
    void addPattern();
    void removePattern(int index);
    void duplicatePattern(int index);
    void setCurrentPattern(int index);
    int getCurrentPattern() const { return currentPattern_; }
    
    // Playback control
    void play();
    void stop();
    void pause();
    bool isPlaying() const { return playing_; }
    
    // Tempo control
    void setTempo(int bpm);
    int getTempo() const { return tempo_; }
    
    // Pattern properties
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
    
    // UI Components
    QVBoxLayout *mainLayout_;
    QHBoxLayout *controlsLayout_;
    QHBoxLayout *patternControlsLayout_;
    
    // Playback controls
    QPushButton *playButton_;
    QPushButton *stopButton_;
    QPushButton *pauseButton_;
    QLabel *tempoLabel_;
    QSlider *tempoSlider_;
    QSpinBox *tempoSpinBox_;
    
    // Pattern controls
    QLabel *patternLabel_;
    QComboBox *patternCombo_;
    QPushButton *addPatternButton_;
    QPushButton *removePatternButton_;
    QPushButton *duplicatePatternButton_;
    
    // Pattern table
    QTableWidget *patternTable_;
    
    // Channel controls
    QGroupBox *channelControlsGroup_;
    std::vector<QCheckBox*> channelMuteCheckboxes_;
    std::vector<QCheckBox*> channelSoloCheckboxes_;
    
    // Data
    std::vector<std::unique_ptr<TrackerPattern>> patterns_;
    int currentPattern_;
    int currentStep_;
    bool playing_;
    bool paused_;
    int tempo_;
    
    // Playback timer
    QTimer *playbackTimer_;
    
    // Constants
    static constexpr int DEFAULT_TEMPO = 120;
    static constexpr int DEFAULT_CHANNELS = 8;
    static constexpr int DEFAULT_STEPS = 64;
    static constexpr int MIN_TEMPO = 60;
    static constexpr int MAX_TEMPO = 200;
};
