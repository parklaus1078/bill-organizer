#include "main_window.h"
#include <QMenuBar>
#include <QToolBar>
#include <QStatusBar>
#include <QLabel>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle("Bill Organizer");
    resize(800, 600);

    setupUi();
    createMenuBar();
    createToolBar();
    createStatusBar();
}

MainWindow::~MainWindow()
{
    // Qt parent-child ownership handles cleanup
}

void MainWindow::setupUi()
{
    // Create central widget
    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    // TODO: Add views and layouts in future implementation
}

void MainWindow::createMenuBar()
{
    // File menu
    QMenu *fileMenu = menuBar()->addMenu(tr("&File"));

    // TODO: Add menu actions in future implementation
}

void MainWindow::createToolBar()
{
    QToolBar *toolbar = addToolBar(tr("Main Toolbar"));

    // TODO: Add toolbar actions in future implementation
}

void MainWindow::createStatusBar()
{
    statusBar()->showMessage(tr("Ready"));
}
