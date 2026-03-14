#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QMainWindow>
#include <QStackedWidget>
#include <QPushButton>
#include <QWidget>

class CardListView;

/**
 * @brief Main application window for Bill Organizer
 *
 * Provides the primary user interface with sidebar navigation
 * and content area. Follows the purple gradient design theme.
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onNavigationClicked(int viewIndex);

private:
    void setupUi();
    void createSidebar();
    QWidget *createContentArea();
    void applyStyles();

    // UI Components
    QWidget *m_sidebar;
    QStackedWidget *m_contentStack;
    CardListView *m_cardListView;
    QList<QPushButton*> m_navButtons;
};

#endif // MAIN_WINDOW_H
