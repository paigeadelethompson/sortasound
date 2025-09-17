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

#include <QApplication>
#include <QMainWindow>
#include <QMessageBox>

#include "window/main.hpp"

/**
 * @brief Main entry point for the SortaSound FM Synthesizer application
 * 
 * Initializes the Qt application and creates the main window for the
 * FM synthesizer interface. This includes setting up the synthesizer
 * engine, keyboard widget, and all user interface components.
 * 
 * @param argc Number of command line arguments
 * @param argv Array of command line argument strings
 * @return Exit code (0 for success, non-zero for error)
 */
int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    app.setApplicationName("SortaSound");
    app.setApplicationVersion("1.0.0");
    app.setOrganizationName("Paige Thompson");
    
    try {
        MainWindow window;
        window.show();
        
        return app.exec();
    }
    catch (const std::exception& e) {
        QMessageBox::critical(nullptr, "Error", 
            QString("Failed to start application: %1").arg(e.what()));
        return -1;
    }
}
