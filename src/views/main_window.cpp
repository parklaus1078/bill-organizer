#include "main_window.h"
#include "card_list_view.h"
#include "../models/payment_card_manager.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle("Bill Organizer");
    resize(1200, 800);

    // Initialize payment card manager
    PaymentCardManager::instance()->initialize();

    setupUi();
    applyStyles();
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

    QHBoxLayout *mainLayout = new QHBoxLayout(centralWidget);
    mainLayout->setSpacing(0);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    // Create sidebar
    createSidebar();
    mainLayout->addWidget(m_sidebar);

    // Create content area
    QWidget *contentArea = createContentArea();
    mainLayout->addWidget(contentArea, 1); // Stretch factor 1
}

void MainWindow::createSidebar()
{
    m_sidebar = new QWidget(this);
    m_sidebar->setFixedWidth(240);
    m_sidebar->setObjectName("sidebar");

    QVBoxLayout *sidebarLayout = new QVBoxLayout(m_sidebar);
    sidebarLayout->setSpacing(0);
    sidebarLayout->setContentsMargins(0, 20, 0, 20);

    // App title
    QLabel *appTitle = new QLabel("Bill\nOrganizer", m_sidebar);
    appTitle->setAlignment(Qt::AlignCenter);
    appTitle->setStyleSheet(
        "font-size: 22px; font-weight: bold; color: white; "
        "padding: 20px; line-height: 1.2;");

    sidebarLayout->addWidget(appTitle);
    sidebarLayout->addSpacing(30);

    // Navigation buttons
    QStringList navItems = {"💳 Payment Cards", "📅 Subscriptions", "📊 Dashboard"};

    for (int i = 0; i < navItems.size(); ++i) {
        QPushButton *navButton = new QPushButton(navItems[i], m_sidebar);
        navButton->setObjectName("navButton");
        navButton->setCursor(Qt::PointingHandCursor);
        navButton->setCheckable(true);
        navButton->setFixedHeight(50);

        connect(navButton, &QPushButton::clicked, this, [this, i]() {
            onNavigationClicked(i);
        });

        m_navButtons.append(navButton);
        sidebarLayout->addWidget(navButton);
    }

    // Set Payment Cards as default selected
    if (!m_navButtons.isEmpty()) {
        m_navButtons[0]->setChecked(true);
    }

    sidebarLayout->addStretch();
}

QWidget *MainWindow::createContentArea()
{
    QWidget *contentArea = new QWidget(this);
    contentArea->setObjectName("contentArea");

    QVBoxLayout *contentLayout = new QVBoxLayout(contentArea);
    contentLayout->setContentsMargins(0, 0, 0, 0);

    // Create stacked widget for different views
    m_contentStack = new QStackedWidget(contentArea);

    // Add views
    m_cardListView = new CardListView(m_contentStack);
    m_contentStack->addWidget(m_cardListView);

    // Placeholder for subscriptions view
    QLabel *subscriptionsPlaceholder = new QLabel("Subscriptions View - Coming Soon", m_contentStack);
    subscriptionsPlaceholder->setAlignment(Qt::AlignCenter);
    subscriptionsPlaceholder->setStyleSheet("font-size: 24px; color: white;");
    m_contentStack->addWidget(subscriptionsPlaceholder);

    // Placeholder for dashboard
    QLabel *dashboardPlaceholder = new QLabel("Dashboard - Coming Soon", m_contentStack);
    dashboardPlaceholder->setAlignment(Qt::AlignCenter);
    dashboardPlaceholder->setStyleSheet("font-size: 24px; color: white;");
    m_contentStack->addWidget(dashboardPlaceholder);

    contentLayout->addWidget(m_contentStack);

    return contentArea;
}

void MainWindow::onNavigationClicked(int viewIndex)
{
    // Update button states
    for (int i = 0; i < m_navButtons.size(); ++i) {
        m_navButtons[i]->setChecked(i == viewIndex);
    }

    // Switch view
    m_contentStack->setCurrentIndex(viewIndex);
}

void MainWindow::applyStyles()
{
    // Main window gradient background
    setStyleSheet(
        "QMainWindow {"
        "  background: qlineargradient(x1:0, y1:0, x2:0, y2:1, "
        "              stop:0 #4A148C, stop:1 #6A1B9A);"
        "}"
        ""
        "QWidget#sidebar {"
        "  background-color: #311B92;"
        "  border-right: 1px solid rgba(255, 255, 255, 0.1);"
        "}"
        ""
        "QPushButton#navButton {"
        "  background: transparent;"
        "  border: none;"
        "  border-left: 4px solid transparent;"
        "  color: white;"
        "  text-align: left;"
        "  padding: 12px 20px;"
        "  font-size: 16px;"
        "}"
        "QPushButton#navButton:hover {"
        "  background: rgba(255, 255, 255, 0.1);"
        "}"
        "QPushButton#navButton:checked {"
        "  background: rgba(233, 30, 99, 0.3);"
        "  border-left-color: #E91E63;"
        "  font-weight: bold;"
        "}"
        ""
        "QWidget#contentArea {"
        "  background: transparent;"
        "}"
    );
}
