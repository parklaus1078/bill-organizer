#include <QApplication>
#include "views/main_window.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // Set application metadata
    app.setOrganizationName("MyCompany");
    app.setApplicationName("BillOrganizer");
    app.setApplicationVersion("1.0.0");

    // Create and show main window
    MainWindow window;
    window.show();

    return app.exec();
}
