#include <QApplication>
#include <QMainWindow>
#include <QMessageBox>
#include <QDebug>

#include "mainwindow.hpp"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    // Set application properties
    app.setApplicationName("FM Synthesizer");
    app.setApplicationVersion("1.0.0");
    app.setOrganizationName("Seroquel");
    
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
