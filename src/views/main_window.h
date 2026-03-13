#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QMainWindow>

/**
 * @brief Main application window for Bill Organizer
 *
 * Provides the primary user interface for managing bills,
 * including adding, editing, deleting, and viewing bills.
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    void setupUi();
    void createMenuBar();
    void createToolBar();
    void createStatusBar();
};

#endif // MAIN_WINDOW_H
