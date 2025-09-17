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

#include "widget/keyboard.hpp"
#include <QPainter>
#include <QMouseEvent>
#include <QResizeEvent>
#include <QApplication>
#include <algorithm>

const QColor KeyboardWidget::WHITE_KEY_COLOR = QColor(255, 255, 255);
const QColor KeyboardWidget::BLACK_KEY_COLOR = QColor(64, 64, 64);
const QColor KeyboardWidget::ACTIVE_KEY_COLOR = QColor(255, 200, 100);
const QColor KeyboardWidget::KEY_BORDER_COLOR = QColor(128, 128, 128);

/**
 * @brief Constructor for KeyboardWidget
 * 
 * Creates a new keyboard widget with default settings. The keyboard displays
 * 4 octaves starting from the current octave, with proper key layout and
 * visual feedback for active notes.
 * 
 * @param parent Parent widget
 */
KeyboardWidget::KeyboardWidget(QWidget *parent)
    : QWidget(parent)
    , alignment_(Qt::AlignLeft)
    , currentOctave_(2)
{
    setMinimumHeight(KEY_HEIGHT);
    setMaximumHeight(KEY_HEIGHT);
    setFocusPolicy(Qt::NoFocus);
    setAttribute(Qt::WA_NoMouseReplay);
    setAttribute(Qt::WA_TransparentForMouseEvents, false);
    setupKeys();
}

/**
 * @brief Set the currently active notes
 * 
 * Updates the visual state of the keyboard to show which notes are currently
 * being played. This affects the color and highlighting of the corresponding keys.
 * 
 * @param notes Set of MIDI note numbers that are currently active
 */
void KeyboardWidget::setActiveNotes(const std::set<int>& notes)
{
    if (activeNotes_ != notes) {
        activeNotes_ = notes;
        update();
    }
}

/**
 * @brief Set the keyboard key mapping
 * 
 * Configures which computer keyboard keys correspond to which MIDI notes.
 * This allows the user to play the synthesizer using their computer keyboard.
 * 
 * @param keyMap Map from Qt key codes to MIDI note numbers
 */
void KeyboardWidget::setKeyMapping(const std::map<Qt::Key, int>& keyMap)
{
    keyToNoteMap_ = keyMap;
    setupKeys();
}

/**
 * @brief Set the keyboard alignment
 * 
 * Controls how the keyboard is positioned within the widget. This affects
 * the layout and positioning of the keys.
 * 
 * @param alignment Qt alignment flags for keyboard positioning
 */
void KeyboardWidget::setAlignment(Qt::Alignment alignment)
{
    alignment_ = alignment;
    setupKeys();
}

/**
 * @brief Set the current octave for the keyboard
 * 
 * Changes the octave range displayed on the keyboard. The keyboard shows
 * 4 octaves starting from the specified octave. This also clears any
 * currently pressed notes.
 * 
 * @param octave The starting octave (0-8, clamped to valid range)
 */
void KeyboardWidget::setCurrentOctave(int octave)
{
    currentOctave_ = qBound(MIN_OCTAVE, octave, MAX_OCTAVE);
    pressedNotes_.clear();
    setupKeys();
    update();
}

/**
 * @brief Setup the keyboard key layout
 * 
 * Calculates and positions all the keys on the keyboard based on the current
 * octave and widget size. This includes determining key sizes, positions,
 * and the proper layout for white and black keys.
 */
void KeyboardWidget::setupKeys()
{
    keys_.clear();
    
    int availableWidth = width();
    int totalWhiteKeys = OCTAVES_DISPLAYED * WHITE_KEYS_PER_OCTAVE;
    
    int whiteKeyWidth = availableWidth / totalWhiteKeys;
    int blackKeyWidth = qMax(3, static_cast<int>(whiteKeyWidth * 0.6));
    
    int totalKeyWidth = totalWhiteKeys * whiteKeyWidth;
    int remainingWidth = availableWidth - totalKeyWidth;
    
    int startX = 0;
    
    std::vector<bool> octavePattern = {false, true, false, true, false, false, true, false, true, false, true, false};
    
    int x = startX;
    int note = currentOctave_ * 12;
    int whiteKeyCount = 0;
    for (int octave = 0; octave < OCTAVES_DISPLAYED; octave++) {
        for (int i = 0; i < KEYS_PER_OCTAVE; i++) {
            KeyInfo key;
            key.note = note;
            key.isBlack = octavePattern[i];
            key.isActive = false;
            
            if (key.isBlack) {
                key.rect = QRect(x - blackKeyWidth/2, 0, blackKeyWidth, BLACK_KEY_HEIGHT);
            } else {
                int currentWhiteKeyWidth = whiteKeyWidth;
                if (whiteKeyCount >= totalWhiteKeys - remainingWidth) {
                    currentWhiteKeyWidth += 1;
                }
                
                key.rect = QRect(x, 0, currentWhiteKeyWidth, KEY_HEIGHT);
                x += currentWhiteKeyWidth;
                whiteKeyCount++;
            }
            
            keys_.push_back(key);
            note++;
        }
    }
    
    updateActiveStates();
}

/**
 * @brief Handle widget resize events
 * 
 * Called when the widget is resized. Recalculates the key layout to fit
 * the new widget dimensions and updates the display.
 * 
 * @param event The resize event
 */
void KeyboardWidget::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    setupKeys();
    update();
}

/**
 * @brief Update the active state of all keys
 * 
 * Updates the visual state of each key based on whether it corresponds to
 * an active note or is currently being pressed by the user.
 */
void KeyboardWidget::updateActiveStates()
{
    for (auto& key : keys_) {
        key.isActive = (activeNotes_.find(key.note) != activeNotes_.end()) || 
                      (pressedNotes_.find(key.note) != pressedNotes_.end());
    }
}


/**
 * @brief Handle widget paint events
 * 
 * Renders the keyboard widget, including all keys, their colors, and
 * any visual feedback for active notes. Also draws note labels on white keys.
 * 
 * @param event The paint event
 */
void KeyboardWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)
    
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    
    updateActiveStates();
    
    for (const auto& key : keys_) {
        if (!key.isBlack) {
            drawKey(painter, key);
        }
    }
    
    for (const auto& key : keys_) {
        if (key.isBlack) {
            drawKey(painter, key);
        }
    }
    painter.setPen(Qt::black);
    painter.setFont(QFont("Arial", 7));
    
    QString noteNames[] = {"C", "D", "E", "F", "G", "A", "B"};
    int noteIndex = 0;
    int currentOctaveForLabel = currentOctave_;
    
    for (const auto& key : keys_) {
        if (!key.isBlack) {
            if (noteIndex % 7 == 0) {
                QString noteText = QString("C%1").arg(currentOctaveForLabel);
                QRect textRect = key.rect.adjusted(0, KEY_HEIGHT - 15, 0, -5);
                painter.drawText(textRect, Qt::AlignCenter, noteText);
            }
            
            noteIndex++;
            if (noteIndex % 7 == 0) {
                currentOctaveForLabel++;
            }
        }
    }
}

/**
 * @brief Draw a single key on the keyboard
 * 
 * Renders an individual key with appropriate colors, borders, and visual
 * effects based on its state (active, pressed, etc.).
 * 
 * @param painter The QPainter object for drawing
 * @param key The key information to draw
 */
void KeyboardWidget::drawKey(QPainter& painter, const KeyInfo& key)
{
    QColor fillColor = key.isBlack ? BLACK_KEY_COLOR : WHITE_KEY_COLOR;
    
    if (key.isActive) {
        fillColor = ACTIVE_KEY_COLOR;
    }
    
    painter.setBrush(fillColor);
    painter.setPen(QPen(KEY_BORDER_COLOR, 1));
    painter.drawRect(key.rect);
    
    if (key.isActive) {
        painter.setBrush(QColor(255, 255, 255, 50));
        painter.setPen(Qt::NoPen);
        painter.drawRect(key.rect.adjusted(2, 2, -2, -2));
    }
}

/**
 * @brief Handle mouse press events
 * 
 * Called when the user presses the mouse button on the keyboard. Determines
 * which key was pressed and emits the appropriate signal.
 * 
 * @param event The mouse press event
 */
void KeyboardWidget::mousePressEvent(QMouseEvent *event)
{
    KeyInfo* key = getKeyAt(event->pos());
    if (key) {
        pressedNotes_.insert(key->note);
        updateActiveStates();
        update();
        emit keyPressed(key->note);
    }
}

/**
 * @brief Handle mouse release events
 * 
 * Called when the user releases the mouse button on the keyboard. Determines
 * which key was released and emits the appropriate signal.
 * 
 * @param event The mouse release event
 */
void KeyboardWidget::mouseReleaseEvent(QMouseEvent *event)
{
    KeyInfo* key = getKeyAt(event->pos());
    if (key) {
        pressedNotes_.erase(key->note);
        updateActiveStates();
        update();
        emit keyReleased(key->note);
    }
}

/**
 * @brief Handle mouse move events
 * 
 * Called when the mouse moves over the keyboard. Handles dragging behavior
 * where the user can drag to play multiple keys or release all keys when
 * dragging outside the keyboard area.
 * 
 * @param event The mouse move event
 */
void KeyboardWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton) {
        KeyInfo* key = getKeyAt(event->pos());
        if (!key) {
            if (!pressedNotes_.empty()) {
                for (int note : pressedNotes_) {
                    emit keyReleased(note);
                }
                pressedNotes_.clear();
                updateActiveStates();
                update();
            }
        }
    }
}

/**
 * @brief Handle keyboard key press events
 * 
 * Called when a key on the computer keyboard is pressed. This is used for
 * computer keyboard input to play notes on the synthesizer.
 * 
 * @param event The key press event
 */
void KeyboardWidget::keyPressEvent(QKeyEvent *event)
{
    QWidget::keyPressEvent(event);
}

/**
 * @brief Handle keyboard key release events
 * 
 * Called when a key on the computer keyboard is released. This is used for
 * computer keyboard input to stop playing notes on the synthesizer.
 * 
 * @param event The key release event
 */
void KeyboardWidget::keyReleaseEvent(QKeyEvent *event)
{
    QWidget::keyReleaseEvent(event);
}

/**
 * @brief Get the key at a specific position
 * 
 * Finds and returns the key information for the key at the given position.
 * This is used for mouse interaction to determine which key was clicked.
 * 
 * @param pos The position to check
 * @return Pointer to the key information, or nullptr if no key at that position
 */
KeyboardWidget::KeyInfo* KeyboardWidget::getKeyAt(const QPoint& pos)
{
    for (auto& key : keys_) {
        if (key.rect.contains(pos)) {
            return &key;
        }
    }
    return nullptr;
}
