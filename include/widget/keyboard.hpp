#pragma once

#include <QWidget>
#include <QPainter>
#include <QMouseEvent>
#include <QTimer>
#include <set>
#include <map>
#include <vector>

class KeyboardWidget : public QWidget
{
    Q_OBJECT

public:
    explicit KeyboardWidget(QWidget *parent = nullptr);
    
    void setActiveNotes(const std::set<int>& notes);
    void setKeyMapping(const std::map<Qt::Key, int>& keyMap);
    void setAlignment(Qt::Alignment alignment);
    void setCurrentOctave(int octave);
    int getCurrentOctave() const { return currentOctave_; }

signals:
    void keyPressed(int note);
    void keyReleased(int note);

protected:
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;

private:
    struct KeyInfo {
        Qt::Key key;
        int note;
        QRect rect;
        bool isBlack;
        bool isActive;
    };
    
    void setupKeys();
    void updateActiveStates();
    KeyInfo* getKeyAt(const QPoint& pos);
    void drawKey(QPainter& painter, const KeyInfo& key);
    
    std::vector<KeyInfo> keys_;
    std::set<int> activeNotes_;
    std::set<int> pressedNotes_; // Track notes that are currently pressed via mouse
    std::map<Qt::Key, int> keyToNoteMap_;
    Qt::Alignment alignment_;
    int currentOctave_;
    
    // Visual constants
    static constexpr int KEY_HEIGHT = 100;
    static constexpr int BLACK_KEY_HEIGHT = 60;
    
    // Colors
    static const QColor WHITE_KEY_COLOR;
    static const QColor BLACK_KEY_COLOR;
    static const QColor ACTIVE_KEY_COLOR;
    static const QColor KEY_BORDER_COLOR;
    
    // Constants
    static constexpr int KEYS_PER_OCTAVE = 12; // 12 keys per octave
    static constexpr int WHITE_KEYS_PER_OCTAVE = 7;
    static constexpr int BLACK_KEYS_PER_OCTAVE = 5;
    static constexpr int OCTAVES_DISPLAYED = 4; // Always show 4 octaves
    static constexpr int MIN_OCTAVE = 0; // C0 (lowest practical octave)
    static constexpr int MAX_OCTAVE = 8; // C8 (highest practical octave for 4-octave display)
};
