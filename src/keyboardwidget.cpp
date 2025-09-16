#include "keyboardwidget.hpp"
#include <QPainter>
#include <QMouseEvent>
#include <QApplication>
#include <algorithm>

// Static color definitions
const QColor KeyboardWidget::WHITE_KEY_COLOR = QColor(255, 255, 255);
const QColor KeyboardWidget::BLACK_KEY_COLOR = QColor(64, 64, 64);
const QColor KeyboardWidget::ACTIVE_KEY_COLOR = QColor(255, 200, 100);
const QColor KeyboardWidget::KEY_BORDER_COLOR = QColor(128, 128, 128);

KeyboardWidget::KeyboardWidget(QWidget *parent)
    : QWidget(parent)
{
    setMinimumHeight(KEY_HEIGHT + 20);
    setMaximumHeight(KEY_HEIGHT + 20);
    setFocusPolicy(Qt::NoFocus); // Don't capture keyboard focus
    setAttribute(Qt::WA_NoMouseReplay); // Don't replay mouse events
    setAttribute(Qt::WA_TransparentForMouseEvents, false); // Allow mouse events for clicking
    setupKeys();
}

void KeyboardWidget::setActiveNotes(const std::set<int>& notes)
{
    // Only update if the notes actually changed
    if (activeNotes_ != notes) {
        activeNotes_ = notes;
        qDebug() << "KeyboardWidget::setActiveNotes called with" << notes.size() << "notes";
        update();
    }
}

void KeyboardWidget::setKeyMapping(const std::map<Qt::Key, int>& keyMap)
{
    keyToNoteMap_ = keyMap;
    setupKeys();
}

void KeyboardWidget::setupKeys()
{
    keys_.clear();
    
    // Create keys for the three octave parts
    int x = 10;
    int keyIndex = 0;
    
    // Part 1: q to ] (12 keys)
    std::vector<Qt::Key> part1Keys = {
        Qt::Key_Q, Qt::Key_W, Qt::Key_E, Qt::Key_R, Qt::Key_T, Qt::Key_Y,
        Qt::Key_U, Qt::Key_I, Qt::Key_O, Qt::Key_P, Qt::Key_BracketLeft, Qt::Key_BracketRight
    };
    
    for (int i = 0; i < 12; i++) {
        KeyInfo key;
        key.key = part1Keys[i];
        key.note = OCTAVE_START + i;
        key.isBlack = (i == 1 || i == 3 || i == 6 || i == 8 || i == 10); // C#, D#, F#, G#, A#
        key.isActive = false;
        
        if (key.isBlack) {
            key.rect = QRect(x - BLACK_KEY_WIDTH/2, 10, BLACK_KEY_WIDTH, BLACK_KEY_HEIGHT);
        } else {
            key.rect = QRect(x, 10, WHITE_KEY_WIDTH, KEY_HEIGHT);
            x += WHITE_KEY_WIDTH;
        }
        
        keys_.push_back(key);
    }
    
    // Add spacing between parts
    x += 20;
    
    // Part 2: a to ' (12 keys)
    std::vector<Qt::Key> part2Keys = {
        Qt::Key_A, Qt::Key_S, Qt::Key_D, Qt::Key_F, Qt::Key_G, Qt::Key_H,
        Qt::Key_J, Qt::Key_K, Qt::Key_L, Qt::Key_Semicolon, Qt::Key_QuoteLeft, Qt::Key_QuoteDbl
    };
    
    for (int i = 0; i < 12; i++) {
        KeyInfo key;
        key.key = part2Keys[i];
        key.note = OCTAVE_START + 12 + i;
        key.isBlack = (i == 1 || i == 3 || i == 6 || i == 8 || i == 10); // C#, D#, F#, G#, A#
        key.isActive = false;
        
        if (key.isBlack) {
            key.rect = QRect(x - BLACK_KEY_WIDTH/2, 10, BLACK_KEY_WIDTH, BLACK_KEY_HEIGHT);
        } else {
            key.rect = QRect(x, 10, WHITE_KEY_WIDTH, KEY_HEIGHT);
            x += WHITE_KEY_WIDTH;
        }
        
        keys_.push_back(key);
    }
    
    // Add spacing between parts
    x += 20;
    
    // Part 3: z to / (11 keys)
    std::vector<Qt::Key> part3Keys = {
        Qt::Key_Z, Qt::Key_X, Qt::Key_C, Qt::Key_V, Qt::Key_B, Qt::Key_N,
        Qt::Key_M, Qt::Key_Comma, Qt::Key_Period, Qt::Key_Slash
    };
    
    for (int i = 0; i < 10; i++) {
        KeyInfo key;
        key.key = part3Keys[i];
        key.note = OCTAVE_START + 24 + i;
        key.isBlack = (i == 1 || i == 3 || i == 6 || i == 8); // C#, D#, F#, G#
        key.isActive = false;
        
        if (key.isBlack) {
            key.rect = QRect(x - BLACK_KEY_WIDTH/2, 10, BLACK_KEY_WIDTH, BLACK_KEY_HEIGHT);
        } else {
            key.rect = QRect(x, 10, WHITE_KEY_WIDTH, KEY_HEIGHT);
            x += WHITE_KEY_WIDTH;
        }
        
        keys_.push_back(key);
    }
    
    // Update active states
    updateActiveStates();
}

void KeyboardWidget::updateActiveStates()
{
    for (auto& key : keys_) {
        key.isActive = (activeNotes_.find(key.note) != activeNotes_.end());
    }
}

void KeyboardWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)
    
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    
    // Update active states
    updateActiveStates();
    
    // Draw white keys first
    for (const auto& key : keys_) {
        if (!key.isBlack) {
            drawKey(painter, key);
        }
    }
    
    // Draw black keys on top
    for (const auto& key : keys_) {
        if (key.isBlack) {
            drawKey(painter, key);
        }
    }
    
    // Draw labels
    painter.setPen(Qt::black);
    painter.setFont(QFont("Arial", 8));
    
    for (const auto& key : keys_) {
        if (!key.isBlack) {
            QString keyText = QKeySequence(key.key).toString();
            QRect textRect = key.rect.adjusted(0, KEY_HEIGHT - 20, 0, -5);
            painter.drawText(textRect, Qt::AlignCenter, keyText);
        }
    }
}

void KeyboardWidget::drawKey(QPainter& painter, const KeyInfo& key)
{
    QColor fillColor = key.isBlack ? BLACK_KEY_COLOR : WHITE_KEY_COLOR;
    
    if (key.isActive) {
        fillColor = ACTIVE_KEY_COLOR;
    }
    
    painter.setBrush(fillColor);
    painter.setPen(QPen(KEY_BORDER_COLOR, 1));
    painter.drawRect(key.rect);
    
    // Add a subtle highlight for active keys
    if (key.isActive) {
        painter.setBrush(QColor(255, 255, 255, 50));
        painter.setPen(Qt::NoPen);
        painter.drawRect(key.rect.adjusted(2, 2, -2, -2));
    }
}

void KeyboardWidget::mousePressEvent(QMouseEvent *event)
{
    KeyInfo* key = getKeyAt(event->pos());
    if (key) {
        emit keyPressed(key->note);
    }
}

void KeyboardWidget::mouseReleaseEvent(QMouseEvent *event)
{
    Q_UNUSED(event)
    // For simplicity, we'll handle note off through the main window
    // This could be enhanced to track mouse press/release per key
}

void KeyboardWidget::keyPressEvent(QKeyEvent *event)
{
    qDebug() << "KeyboardWidget::keyPressEvent - key:" << event->key() << "passing to parent";
    QWidget::keyPressEvent(event); // Pass to parent (MainWindow)
}

void KeyboardWidget::keyReleaseEvent(QKeyEvent *event)
{
    qDebug() << "KeyboardWidget::keyReleaseEvent - key:" << event->key() << "passing to parent";
    QWidget::keyReleaseEvent(event); // Pass to parent (MainWindow)
}

KeyboardWidget::KeyInfo* KeyboardWidget::getKeyAt(const QPoint& pos)
{
    for (auto& key : keys_) {
        if (key.rect.contains(pos)) {
            return &key;
        }
    }
    return nullptr;
}
