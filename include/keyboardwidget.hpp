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

signals:
    void keyPressed(int note);
    void keyReleased(int note);

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
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
    std::map<Qt::Key, int> keyToNoteMap_;
    
    // Visual constants
    static constexpr int KEY_HEIGHT = 100;
    static constexpr int WHITE_KEY_WIDTH = 40;
    static constexpr int BLACK_KEY_WIDTH = 25;
    static constexpr int BLACK_KEY_HEIGHT = 60;
    
    // Colors
    static const QColor WHITE_KEY_COLOR;
    static const QColor BLACK_KEY_COLOR;
    static const QColor ACTIVE_KEY_COLOR;
    static const QColor KEY_BORDER_COLOR;
    
    // Constants
    static constexpr int OCTAVE_START = 60; // Middle C (C4)
};
