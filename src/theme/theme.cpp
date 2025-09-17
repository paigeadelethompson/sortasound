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

#include "theme/theme.hpp"
#include <QMap>
#include <QDebug>

ThemeManager& ThemeManager::getInstance()
{
    static ThemeManager instance;
    return instance;
}

// Theme is fixed to Catppuccin Frappé - no theme switching needed

void ThemeManager::applyTheme(QApplication* app)
{
    if (!app) return;
    
    // Initialize Catppuccin Frappé colors
    initializeFrappeColors();
    
    // Apply color palette
    app->setPalette(generatePalette());
    
    // Apply stylesheet
    app->setStyleSheet(generateStylesheet() + generateCustomStyles());
}

QColor ThemeManager::getColor(const QString& colorName) const
{
    static QMap<QString, QColor> colorMap;
    
    if (colorMap.isEmpty()) {
        // Base colors
        colorMap["base"] = currentPalette_.base;
        colorMap["mantle"] = currentPalette_.mantle;
        colorMap["crust"] = currentPalette_.crust;
        
        // Surface colors
        colorMap["surface0"] = currentPalette_.surface0;
        colorMap["surface1"] = currentPalette_.surface1;
        colorMap["surface2"] = currentPalette_.surface2;
        
        // Overlay colors
        colorMap["overlay0"] = currentPalette_.overlay0;
        colorMap["overlay1"] = currentPalette_.overlay1;
        colorMap["overlay2"] = currentPalette_.overlay2;
        
        // Text colors
        colorMap["text"] = currentPalette_.text;
        colorMap["subtext1"] = currentPalette_.subtext1;
        colorMap["subtext0"] = currentPalette_.subtext0;
        
        // Accent colors
        colorMap["rosewater"] = currentPalette_.rosewater;
        colorMap["flamingo"] = currentPalette_.flamingo;
        colorMap["pink"] = currentPalette_.pink;
        colorMap["mauve"] = currentPalette_.mauve;
        colorMap["red"] = currentPalette_.red;
        colorMap["maroon"] = currentPalette_.maroon;
        colorMap["peach"] = currentPalette_.peach;
        colorMap["yellow"] = currentPalette_.yellow;
        colorMap["green"] = currentPalette_.green;
        colorMap["teal"] = currentPalette_.teal;
        colorMap["sky"] = currentPalette_.sky;
        colorMap["sapphire"] = currentPalette_.sapphire;
        colorMap["blue"] = currentPalette_.blue;
        colorMap["lavender"] = currentPalette_.lavender;
    }
    
    return colorMap.value(colorName, QColor(255, 0, 255)); // Magenta for unknown colors
}

QPalette ThemeManager::getPalette() const
{
    return generatePalette();
}

QString ThemeManager::getStylesheet() const
{
    return generateStylesheet();
}

QString ThemeManager::getCustomStyles() const
{
    return generateCustomStyles();
}

void ThemeManager::initializeFrappeColors()
{
    // Catppuccin Frappé colors - muted aesthetic
    // https://catppuccin.com/palette/
    
    // Base colors
    currentPalette_.base = QColor("#232634");        // rgb(35, 38, 52)
    currentPalette_.mantle = QColor("#1e2030");      // rgb(30, 32, 48)
    currentPalette_.crust = QColor("#181926");       // rgb(24, 25, 38)
    
    // Surface colors
    currentPalette_.surface0 = QColor("#292c3c");    // rgb(41, 44, 60)
    currentPalette_.surface1 = QColor("#303446");    // rgb(48, 52, 70)
    currentPalette_.surface2 = QColor("#414559");    // rgb(65, 69, 89)
    
    // Overlay colors
    currentPalette_.overlay0 = QColor("#51576d");    // rgb(81, 87, 109)
    currentPalette_.overlay1 = QColor("#626880");    // rgb(98, 104, 128)
    currentPalette_.overlay2 = QColor("#737994");    // rgb(115, 121, 148)
    
    // Text colors
    currentPalette_.text = QColor("#c6d0f5");        // rgb(198, 208, 245)
    currentPalette_.subtext1 = QColor("#b5bfe2");    // rgb(181, 191, 226)
    currentPalette_.subtext0 = QColor("#a5adce");    // rgb(165, 173, 206)
    
    // Accent colors
    currentPalette_.rosewater = QColor("#f2d5cf");   // rgb(242, 213, 207)
    currentPalette_.flamingo = QColor("#eebebe");    // rgb(238, 190, 190)
    currentPalette_.pink = QColor("#f4b8e4");        // rgb(244, 184, 228)
    currentPalette_.mauve = QColor("#ca9ee6");       // rgb(202, 158, 230)
    currentPalette_.red = QColor("#e78284");         // rgb(231, 130, 132)
    currentPalette_.maroon = QColor("#ea999c");      // rgb(234, 153, 156)
    currentPalette_.peach = QColor("#ef9f76");       // rgb(239, 159, 118)
    currentPalette_.yellow = QColor("#e5c890");      // rgb(229, 200, 144)
    currentPalette_.green = QColor("#a6d189");       // rgb(166, 209, 137)
    currentPalette_.teal = QColor("#81c8be");        // rgb(129, 200, 190)
    currentPalette_.sky = QColor("#99d1db");         // rgb(153, 209, 219)
    currentPalette_.sapphire = QColor("#85c1dc");    // rgb(133, 193, 220)
    currentPalette_.blue = QColor("#8caaee");        // rgb(140, 170, 238)
    currentPalette_.lavender = QColor("#babbf1");    // rgb(186, 187, 241)
}

void ThemeManager::initializeMochaColors()
{
    // Catppuccin Mocha colors - original dark theme
    // https://catppuccin.com/palette/
    
    // Base colors
    currentPalette_.base = QColor("#1e1e2e");        // rgb(30, 30, 46)
    currentPalette_.mantle = QColor("#181825");      // rgb(24, 24, 37)
    currentPalette_.crust = QColor("#11111b");       // rgb(17, 17, 27)
    
    // Surface colors
    currentPalette_.surface0 = QColor("#313244");    // rgb(49, 50, 68)
    currentPalette_.surface1 = QColor("#45475a");    // rgb(69, 71, 90)
    currentPalette_.surface2 = QColor("#585b70");    // rgb(88, 91, 112)
    
    // Overlay colors
    currentPalette_.overlay0 = QColor("#6c7086");    // rgb(108, 112, 134)
    currentPalette_.overlay1 = QColor("#7f849c");    // rgb(127, 132, 156)
    currentPalette_.overlay2 = QColor("#9399b2");    // rgb(147, 153, 178)
    
    // Text colors
    currentPalette_.text = QColor("#cdd6f4");        // rgb(205, 214, 244)
    currentPalette_.subtext1 = QColor("#bac2de");    // rgb(186, 194, 222)
    currentPalette_.subtext0 = QColor("#a6adc8");    // rgb(166, 173, 200)
    
    // Accent colors
    currentPalette_.rosewater = QColor("#f5e0dc");   // rgb(245, 224, 220)
    currentPalette_.flamingo = QColor("#f2cdcd");    // rgb(242, 205, 205)
    currentPalette_.pink = QColor("#f5c2e7");        // rgb(245, 194, 231)
    currentPalette_.mauve = QColor("#cba6f7");       // rgb(203, 166, 247)
    currentPalette_.red = QColor("#f38ba8");         // rgb(243, 139, 168)
    currentPalette_.maroon = QColor("#eba0ac");      // rgb(235, 160, 172)
    currentPalette_.peach = QColor("#fab387");       // rgb(250, 179, 135)
    currentPalette_.yellow = QColor("#f9e2af");      // rgb(249, 226, 175)
    currentPalette_.green = QColor("#a6e3a1");       // rgb(166, 227, 161)
    currentPalette_.teal = QColor("#94e2d5");        // rgb(148, 226, 213)
    currentPalette_.sky = QColor("#89dceb");         // rgb(137, 220, 235)
    currentPalette_.sapphire = QColor("#74c7ec");    // rgb(116, 199, 236)
    currentPalette_.blue = QColor("#89b4fa");        // rgb(137, 180, 250)
    currentPalette_.lavender = QColor("#b4befe");    // rgb(180, 190, 254)
}

void ThemeManager::initializeLatteColors()
{
    // Catppuccin Latte colors - light theme
    // https://catppuccin.com/palette/
    
    // Base colors
    currentPalette_.base = QColor("#eff1f5");        // rgb(239, 241, 245)
    currentPalette_.mantle = QColor("#e6e9ef");      // rgb(230, 233, 239)
    currentPalette_.crust = QColor("#dce0e8");       // rgb(220, 224, 232)
    
    // Surface colors
    currentPalette_.surface0 = QColor("#ccd0da");    // rgb(204, 208, 218)
    currentPalette_.surface1 = QColor("#bcc0cc");    // rgb(188, 192, 204)
    currentPalette_.surface2 = QColor("#acb0be");    // rgb(172, 176, 190)
    
    // Overlay colors
    currentPalette_.overlay0 = QColor("#9ca0b0");    // rgb(156, 160, 176)
    currentPalette_.overlay1 = QColor("#8c8fa1");    // rgb(140, 143, 161)
    currentPalette_.overlay2 = QColor("#7c7f93");    // rgb(124, 127, 147)
    
    // Text colors
    currentPalette_.text = QColor("#4c4f69");        // rgb(76, 79, 105)
    currentPalette_.subtext1 = QColor("#5c5f77");    // rgb(92, 95, 119)
    currentPalette_.subtext0 = QColor("#6c6f85");    // rgb(108, 111, 133)
    
    // Accent colors
    currentPalette_.rosewater = QColor("#dc8a78");   // rgb(220, 138, 120)
    currentPalette_.flamingo = QColor("#dd7878");    // rgb(221, 120, 120)
    currentPalette_.pink = QColor("#ea76cb");        // rgb(234, 118, 203)
    currentPalette_.mauve = QColor("#8839ef");       // rgb(136, 57, 239)
    currentPalette_.red = QColor("#d20f39");         // rgb(210, 15, 57)
    currentPalette_.maroon = QColor("#e64553");      // rgb(230, 69, 83)
    currentPalette_.peach = QColor("#fe640b");       // rgb(254, 100, 11)
    currentPalette_.yellow = QColor("#df8e1d");      // rgb(223, 142, 29)
    currentPalette_.green = QColor("#40a02b");       // rgb(64, 160, 43)
    currentPalette_.teal = QColor("#179299");        // rgb(23, 146, 153)
    currentPalette_.sky = QColor("#04a5e5");         // rgb(4, 165, 229)
    currentPalette_.sapphire = QColor("#209fb5");    // rgb(32, 159, 181)
    currentPalette_.blue = QColor("#1e66f5");        // rgb(30, 102, 245)
    currentPalette_.lavender = QColor("#7287fd");    // rgb(114, 135, 253)
}

void ThemeManager::initializeMacchiatoColors()
{
    // Catppuccin Macchiato colors - medium contrast
    // https://catppuccin.com/palette/
    
    // Base colors
    currentPalette_.base = QColor("#24273a");        // rgb(36, 39, 58)
    currentPalette_.mantle = QColor("#1e2030");      // rgb(30, 32, 48)
    currentPalette_.crust = QColor("#181926");       // rgb(24, 25, 38)
    
    // Surface colors
    currentPalette_.surface0 = QColor("#363a4f");    // rgb(54, 58, 79)
    currentPalette_.surface1 = QColor("#494d64");    // rgb(73, 77, 100)
    currentPalette_.surface2 = QColor("#5b6078");    // rgb(91, 96, 120)
    
    // Overlay colors
    currentPalette_.overlay0 = QColor("#6e738d");    // rgb(110, 115, 141)
    currentPalette_.overlay1 = QColor("#8087a2");    // rgb(128, 135, 162)
    currentPalette_.overlay2 = QColor("#939ab7");    // rgb(147, 154, 183)
    
    // Text colors
    currentPalette_.text = QColor("#cad3f5");        // rgb(202, 211, 245)
    currentPalette_.subtext1 = QColor("#b8c0e0");    // rgb(184, 192, 224)
    currentPalette_.subtext0 = QColor("#a5adcb");    // rgb(165, 173, 203)
    
    // Accent colors
    currentPalette_.rosewater = QColor("#f4dbd6");   // rgb(244, 219, 214)
    currentPalette_.flamingo = QColor("#f0c6c6");    // rgb(240, 198, 198)
    currentPalette_.pink = QColor("#f5bde6");        // rgb(245, 189, 230)
    currentPalette_.mauve = QColor("#c6a0f6");       // rgb(198, 160, 246)
    currentPalette_.red = QColor("#ed8796");         // rgb(237, 135, 150)
    currentPalette_.maroon = QColor("#ee99a0");      // rgb(238, 153, 160)
    currentPalette_.peach = QColor("#f5a97f");       // rgb(245, 169, 127)
    currentPalette_.yellow = QColor("#eed49f");      // rgb(238, 212, 159)
    currentPalette_.green = QColor("#a6da95");       // rgb(166, 218, 149)
    currentPalette_.teal = QColor("#8bd5ca");        // rgb(139, 213, 202)
    currentPalette_.sky = QColor("#91d7e3");         // rgb(145, 215, 227)
    currentPalette_.sapphire = QColor("#7dc4e4");    // rgb(125, 196, 228)
    currentPalette_.blue = QColor("#8aadf4");        // rgb(138, 173, 244)
    currentPalette_.lavender = QColor("#b7bdf8");    // rgb(183, 189, 248)
}

QPalette ThemeManager::generatePalette() const
{
    QPalette palette;
    
    // Set base colors
    palette.setColor(QPalette::Window, currentPalette_.base);
    palette.setColor(QPalette::WindowText, currentPalette_.text);
    palette.setColor(QPalette::Base, currentPalette_.surface0);
    palette.setColor(QPalette::AlternateBase, currentPalette_.surface1);
    palette.setColor(QPalette::ToolTipBase, currentPalette_.surface2);
    palette.setColor(QPalette::ToolTipText, currentPalette_.text);
    palette.setColor(QPalette::Text, currentPalette_.text);
    palette.setColor(QPalette::Button, currentPalette_.surface1);
    palette.setColor(QPalette::ButtonText, currentPalette_.text);
    palette.setColor(QPalette::BrightText, currentPalette_.red);
    palette.setColor(QPalette::Link, currentPalette_.blue);
    palette.setColor(QPalette::Highlight, currentPalette_.blue);
    palette.setColor(QPalette::HighlightedText, currentPalette_.base);
    
    return palette;
}

QString ThemeManager::generateStylesheet() const
{
    return QString(R"(
        /* Main window styling */
        QMainWindow {
            background-color: %1;
            color: %2;
        }
        
        /* Tab widget styling */
        QTabWidget::pane {
            border: 1px solid %3;
            background-color: %4;
        }
        
        QTabBar::tab {
            background-color: %5;
            color: %6;
            padding: 8px 16px;
            margin-right: 2px;
            border-top-left-radius: 4px;
            border-top-right-radius: 4px;
        }
        
        QTabBar::tab:selected {
            background-color: %4;
            color: %2;
        }
        
        QTabBar::tab:hover {
            background-color: %7;
        }
        
        /* Group box styling */
        QGroupBox {
            font-weight: bold;
            border: none;
            border-radius: 0px;
            margin-top: 1ex;
            padding-top: 10px;
            color: %2;
        }
        
        QGroupBox::title {
            subcontrol-origin: margin;
            left: 10px;
            padding: 0 5px 0 5px;
            color: %8;
        }
        
        /* Slider styling */
        QSlider::groove:horizontal {
            border: none;
            height: 8px;
            background: %5;
            border-radius: 0px;
        }
        
        QSlider::handle:horizontal {
            background: %2;
            border: none;
            width: 18px;
            margin: -2px 0;
            border-radius: 0px;
        }
        
        QSlider::handle:horizontal:hover {
            background: %6;
        }
        
        QSlider::groove:vertical {
            border: none;
            width: 8px;
            background: %5;
            border-radius: 0px;
        }
        
        QSlider::handle:vertical {
            background: %2;
            border: none;
            height: 18px;
            margin: 0 -2px;
            border-radius: 0px;
        }
        
        QSlider::handle:vertical:hover {
            background: %6;
        }
        
        /* Combo box styling */
        QComboBox {
            border: none;
            border-radius: 0px;
            padding: 5px;
            background-color: %4;
            color: %2;
        }
        
        QComboBox::drop-down {
            subcontrol-origin: padding;
            subcontrol-position: top right;
            width: 20px;
            border: none;
            border-top-right-radius: 0px;
            border-bottom-right-radius: 0px;
        }
        
        QComboBox::down-arrow {
            image: none;
            border-left: 5px solid transparent;
            border-right: 5px solid transparent;
            border-top: 6px solid #a6d189;
            width: 0px;
            height: 0px;
        }
        
        QComboBox QAbstractItemView {
            border: none;
            background-color: %4;
            selection-background-color: %5;
            color: %2;
        }
        
        /* Spin box styling */
        QSpinBox, QDoubleSpinBox {
            border: none;
            border-radius: 0px;
            padding: 5px;
            background-color: %4;
            color: %2;
        }
        
        QSpinBox::up-button, QDoubleSpinBox::up-button {
            subcontrol-origin: border;
            subcontrol-position: top right;
            width: 20px;
            border: none;
            border-top-right-radius: 0px;
            background-color: %4;
        }
        
        QSpinBox::up-button:hover, QDoubleSpinBox::up-button:hover {
            background-color: %5;
        }
        
        QSpinBox::down-button, QDoubleSpinBox::down-button {
            subcontrol-origin: border;
            subcontrol-position: bottom right;
            width: 20px;
            border: none;
            border-bottom-right-radius: 0px;
            background-color: %4;
        }
        
        QSpinBox::down-button:hover, QDoubleSpinBox::down-button:hover {
            background-color: %5;
        }
        
        QSpinBox::up-arrow, QDoubleSpinBox::up-arrow {
            image: none;
            border-left: 5px solid transparent;
            border-right: 5px solid transparent;
            border-bottom: 6px solid #a6d189;
            width: 0px;
            height: 0px;
        }
        
        QSpinBox::down-arrow, QDoubleSpinBox::down-arrow {
            image: none;
            border-left: 5px solid transparent;
            border-right: 5px solid transparent;
            border-top: 6px solid #a6d189;
            width: 0px;
            height: 0px;
        }
        
        
        
        /* Push button styling */
        QPushButton {
            background-color: %5;
            border: none;
            border-radius: 0px;
            padding: 6px 12px;
            color: %2;
            font-weight: bold;
        }
        
        QPushButton:hover {
            background-color: %7;
        }
        
        QPushButton:pressed {
            background-color: %12;
        }
        
        /* Label styling */
        QLabel {
            color: %2;
        }
        
        /* Scroll area styling */
        QScrollArea {
            border: none;
            background-color: %4;
        }
        
        QScrollBar:vertical {
            background-color: %5;
            width: 12px;
            border-radius: 0px;
        }
        
        QScrollBar::handle:vertical {
            background-color: %13;
            border-radius: 0px;
            min-height: 20px;
        }
        
        QScrollBar::handle:vertical:hover {
            background-color: %14;
        }
        
        QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {
            height: 0px;
        }
        
        QScrollBar:horizontal {
            background-color: %5;
            height: 12px;
            border-radius: 0px;
        }
        
        QScrollBar::handle:horizontal {
            background-color: %13;
            border-radius: 0px;
            min-width: 20px;
        }
        
        QScrollBar::handle:horizontal:hover {
            background-color: %14;
        }
        
        QScrollBar::add-line:horizontal, QScrollBar::sub-line:horizontal {
            width: 0px;
        }
    )")
    .arg(currentPalette_.base.name())           // %1
    .arg(currentPalette_.text.name())           // %2
    .arg(currentPalette_.overlay0.name())       // %3
    .arg(currentPalette_.surface0.name())       // %4
    .arg(currentPalette_.surface1.name())       // %5
    .arg(currentPalette_.subtext0.name())       // %6
    .arg(currentPalette_.surface2.name())       // %7
    .arg(currentPalette_.mauve.name())          // %8
    .arg(currentPalette_.blue.name())           // %9
    .arg(currentPalette_.sky.name())            // %10
    .arg(currentPalette_.blue.name())           // %11
    .arg(currentPalette_.overlay1.name())       // %12
    .arg(currentPalette_.overlay1.name())       // %13
    .arg(currentPalette_.overlay2.name());      // %14
}

QString ThemeManager::generateCustomStyles() const
{
    return QString(R"(
        /* Custom SortaSound UI elements */
        
        /* Synthesizer tab styling */
        QTabWidget#synthesizerTabWidget {
            background-color: %1;
        }
        
        /* Keyboard widget custom styling */
        KeyboardWidget {
            background-color: %2;
            border: 2px solid %3;
            border-radius: 8px;
        }
        
        /* Tracker widget styling */
        TrackerWidget {
            background-color: %2;
            border: 1px solid %3;
            border-radius: 4px;
        }
        
        /* Control panel styling */
        QWidget[objectName="controlsPanel"] {
            background-color: %4;
            border: 1px solid %3;
            border-radius: 6px;
            padding: 8px;
        }
        
        /* Effect sliders - keep tracks colorful but handles blend */
        QSlider[objectName="reverbSlider"]::groove:horizontal {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:0,
                stop:0 %5, stop:1 %6);
        }
        
        QSlider[objectName="chorusSlider"]::groove:horizontal {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:0,
                stop:0 %7, stop:1 %8);
        }
        
        QSlider[objectName="distortionSlider"]::groove:horizontal {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:0,
                stop:0 %9, stop:1 %10);
        }
        
        /* Volume, pitch bend, and mod wheel sliders use same neutral color as other sliders */
        QSlider[objectName="volumeSlider"]::groove:vertical {
            background: %5;
        }
        
        QSlider[objectName="pitchBendSlider"]::groove:vertical {
            background: %5;
        }
        
        QSlider[objectName="modWheelSlider"]::groove:vertical {
            background: %5;
        }
        
        /* Add synthesizer button styling */
        QPushButton[objectName="addSynthButton"] {
            background-color: %5;
            border: none;
            border-radius: 0px;
            font-size: 14px;
            font-weight: bold;
            color: %2;
            min-width: 24px;
            min-height: 24px;
            padding: 2px;
        }
        
        QPushButton[objectName="addSynthButton"]:hover {
            background-color: %7;
        }
        
        /* Remove custom styling - use default simple styling */
    )")
    .arg(currentPalette_.base.name())           // %1
    .arg(currentPalette_.surface0.name())       // %2
    .arg(currentPalette_.overlay0.name())       // %3
    .arg(currentPalette_.surface1.name())       // %4
    .arg(currentPalette_.surface1.name())       // %5 - neutral slider color
    .arg(currentPalette_.teal.name())           // %6 - reverb end
    .arg(currentPalette_.blue.name())           // %7 - chorus start
    .arg(currentPalette_.sky.name())            // %8 - chorus end
    .arg(currentPalette_.red.name())            // %9 - distortion start
    .arg(currentPalette_.peach.name())          // %10 - distortion end
    .arg(currentPalette_.mauve.name())          // %11 - algorithm start
    .arg(currentPalette_.lavender.name())       // %12 - algorithm end
    .arg(currentPalette_.pink.name())           // %13 - algorithm border
    .arg(currentPalette_.yellow.name())         // %14 - volume start
    .arg(currentPalette_.peach.name())          // %15 - volume end
    .arg(currentPalette_.red.name())            // %16 - pitch bend low
    .arg(currentPalette_.base.name())           // %17 - pitch bend center
    .arg(currentPalette_.green.name())          // %18 - pitch bend high
    .arg(currentPalette_.overlay0.name())       // %19 - mod wheel start
    .arg(currentPalette_.blue.name())           // %20 - mod wheel end
    .arg(currentPalette_.green.name())          // %21 - add button bg
    .arg(currentPalette_.teal.name())           // %22 - add button border
    .arg(currentPalette_.base.name())           // %23 - add button text
    .arg(currentPalette_.teal.name())           // %24 - add button hover
    .arg(currentPalette_.yellow.name())         // %25 - octave start
    .arg(currentPalette_.peach.name())          // %26 - octave end
    .arg(currentPalette_.maroon.name())         // %27 - octave border
    .arg(currentPalette_.sapphire.name())       // %28 - preset start
    .arg(currentPalette_.sky.name())            // %29 - preset end
    .arg(currentPalette_.blue.name())           // %30 - preset border
    .arg(currentPalette_.lavender.name())       // %31 - channel start
    .arg(currentPalette_.mauve.name())          // %32 - channel end
    .arg(currentPalette_.pink.name());          // %33 - channel border
}
