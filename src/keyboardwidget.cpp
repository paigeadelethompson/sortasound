#include "keyboardwidget.hpp"
#include <QPainter>
#include <QMouseEvent>
#include <QResizeEvent>
#include <QApplication>
#include <algorithm>

// Static color definitions
const QColor KeyboardWidget::WHITE_KEY_COLOR = QColor(255, 255, 255);
const QColor KeyboardWidget::BLACK_KEY_COLOR = QColor(64, 64, 64);
const QColor KeyboardWidget::ACTIVE_KEY_COLOR = QColor(255, 200, 100);
const QColor KeyboardWidget::KEY_BORDER_COLOR = QColor(128, 128, 128);

KeyboardWidget::KeyboardWidget(QWidget *parent)
    : QWidget(parent)
    , alignment_(Qt::AlignLeft)
    , currentOctave_(2) // Start at C2
{
    setMinimumHeight(KEY_HEIGHT);
    setMaximumHeight(KEY_HEIGHT);
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

void KeyboardWidget::setAlignment(Qt::Alignment alignment)
{
    alignment_ = alignment;
    setupKeys();
}

void KeyboardWidget::setCurrentOctave(int octave)
{
    currentOctave_ = qBound(MIN_OCTAVE, octave, MAX_OCTAVE);
    setupKeys();
}

void KeyboardWidget::setupKeys()
{
    keys_.clear();
    
    // Calculate available width and key dimensions for 4 octaves
    int availableWidth = width(); // Use full width, no margins
    int totalWhiteKeys = OCTAVES_DISPLAYED * WHITE_KEYS_PER_OCTAVE; // 4 octaves * 7 white keys = 28 white keys
    
    // Use full available width for keys to eliminate right padding
    int whiteKeyWidth = availableWidth / totalWhiteKeys; // Use exact division to fill full width
    int blackKeyWidth = qMax(3, static_cast<int>(whiteKeyWidth * 0.6)); // Minimum 3px for black keys
    
    qDebug() << "KeyboardWidget::setupKeys - availableWidth:" << availableWidth << "whiteKeyWidth:" << whiteKeyWidth;
    
    // Always start at left edge to eliminate padding
    int startX = 0;
    
    // Define the pattern of white and black keys in an octave
    std::vector<bool> octavePattern = {false, true, false, true, false, false, true, false, true, false, true, false}; // C, C#, D, D#, E, F, F#, G, G#, A, A#, B
    
    int x = startX;
    int note = currentOctave_ * 12; // Calculate starting note for current octave
    
    // Create keys for 4 octaves
    for (int octave = 0; octave < OCTAVES_DISPLAYED; octave++) {
        for (int i = 0; i < KEYS_PER_OCTAVE; i++) {
            KeyInfo key;
            key.note = note;
            key.isBlack = octavePattern[i];
            key.isActive = false;
            
            if (key.isBlack) {
                // Position black keys between white keys
                key.rect = QRect(x - blackKeyWidth/2, 0, blackKeyWidth, BLACK_KEY_HEIGHT);
            } else {
                // White keys
                key.rect = QRect(x, 0, whiteKeyWidth, KEY_HEIGHT);
                x += whiteKeyWidth;
            }
            
            keys_.push_back(key);
            note++;
        }
    }
    
    // Update active states
    updateActiveStates();
}

void KeyboardWidget::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    qDebug() << "KeyboardWidget::resizeEvent - new size:" << event->size();
    setupKeys(); // Recalculate key positions when window is resized
    update(); // Force immediate repaint
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
    
    // Draw note labels for white keys (only show C notes to avoid clutter)
    painter.setPen(Qt::black);
    painter.setFont(QFont("Arial", 7));
    
    QString noteNames[] = {"C", "D", "E", "F", "G", "A", "B"};
    int noteIndex = 0;
    int currentOctaveForLabel = currentOctave_;
    
    for (const auto& key : keys_) {
        if (!key.isBlack) {
            // Only show C notes to avoid clutter
            if (noteIndex % 7 == 0) { // C notes are at positions 0, 7, 14, 21
                QString noteText = QString("C%1").arg(currentOctaveForLabel);
                QRect textRect = key.rect.adjusted(0, KEY_HEIGHT - 15, 0, -5);
                painter.drawText(textRect, Qt::AlignCenter, noteText);
            }
            
            noteIndex++;
            // Move to next octave after 7 white keys
            if (noteIndex % 7 == 0) {
                currentOctaveForLabel++;
            }
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
    KeyInfo* key = getKeyAt(event->pos());
    if (key) {
        emit keyReleased(key->note);
    }
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
