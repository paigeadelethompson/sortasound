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

#include <QColor>
#include <QString>
#include <QPalette>
#include <QApplication>
#include <memory>

/**
 * @brief Theme manager for SortaSound application
 * 
 * Provides Catppuccin Frappé theming for the application.
 * Simple, fixed theme implementation with no user switching.
 */
class ThemeManager
{
public:
    /**
     * @brief Get the singleton instance of ThemeManager
     * @return Reference to the ThemeManager instance
     */
    static ThemeManager& getInstance();

    /**
     * @brief Apply the current theme to the application
     * @param app QApplication instance to apply theme to
     */
    void applyTheme(QApplication* app);

    /**
     * @brief Get a color by name from the current theme
     * @param colorName Name of the color (e.g., "base", "text", "blue")
     * @return QColor object
     */
    QColor getColor(const QString& colorName) const;

    /**
     * @brief Get the complete color palette for the current theme
     * @return QPalette object
     */
    QPalette getPalette() const;

    /**
     * @brief Get CSS stylesheet for the current theme
     * @return QString containing CSS styles
     */
    QString getStylesheet() const;

    /**
     * @brief Get custom UI element styles
     * @return QString containing custom CSS for UI elements
     */
    QString getCustomStyles() const;

private:
    ThemeManager() = default;
    ~ThemeManager() = default;
    ThemeManager(const ThemeManager&) = delete;
    ThemeManager& operator=(const ThemeManager&) = delete;

    // Fixed to Catppuccin Frappé theme

    /**
     * @brief Initialize Catppuccin Frappé color palette
     */
    void initializeFrappeColors();

    /**
     * @brief Initialize Catppuccin Mocha color palette
     */
    void initializeMochaColors();

    /**
     * @brief Initialize Catppuccin Latte color palette
     */
    void initializeLatteColors();

    /**
     * @brief Initialize Catppuccin Macchiato color palette
     */
    void initializeMacchiatoColors();

    /**
     * @brief Color storage for current theme
     */
    struct ColorPalette {
        // Base colors
        QColor base;        // Main background
        QColor mantle;      // Secondary background
        QColor crust;       // Tertiary background
        
        // Surface colors
        QColor surface0;    // Surface level 0
        QColor surface1;    // Surface level 1
        QColor surface2;    // Surface level 2
        
        // Overlay colors
        QColor overlay0;    // Overlay level 0
        QColor overlay1;    // Overlay level 1
        QColor overlay2;    // Overlay level 2
        
        // Text colors
        QColor text;        // Primary text
        QColor subtext1;    // Secondary text
        QColor subtext0;    // Tertiary text
        
        // Accent colors
        QColor rosewater;   // Rosewater accent
        QColor flamingo;    // Flamingo accent
        QColor pink;        // Pink accent
        QColor mauve;       // Mauve accent
        QColor red;         // Red accent
        QColor maroon;      // Maroon accent
        QColor peach;       // Peach accent
        QColor yellow;      // Yellow accent
        QColor green;       // Green accent
        QColor teal;        // Teal accent
        QColor sky;         // Sky accent
        QColor sapphire;    // Sapphire accent
        QColor blue;        // Blue accent
        QColor lavender;    // Lavender accent
    };

    ColorPalette currentPalette_;

    /**
     * @brief Generate QPalette from current color palette
     * @return QPalette object
     */
    QPalette generatePalette() const;

    /**
     * @brief Generate CSS stylesheet from current color palette
     * @return QString containing CSS
     */
    QString generateStylesheet() const;

    /**
     * @brief Generate custom UI element styles
     * @return QString containing custom CSS
     */
    QString generateCustomStyles() const;
};
