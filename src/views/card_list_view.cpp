#include "card_list_view.h"
#include "card_form_dialog.h"
#include "../models/payment_card_manager.h"
#include "../models/payment_card_list_model.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QListWidgetItem>

CardListView::CardListView(QWidget *parent)
    : QWidget(parent)
    , m_manager(PaymentCardManager::instance())
    , m_model(new PaymentCardListModel(this))
{
    setupUI();
    setupConnections();
    loadCards();
}

void CardListView::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(20);
    mainLayout->setContentsMargins(40, 40, 40, 40);

    // Header with title and add button
    QHBoxLayout *headerLayout = new QHBoxLayout();

    m_titleLabel = new QLabel("Payment Cards", this);
    m_titleLabel->setStyleSheet(
        "font-size: 32px; font-weight: bold; color: white;");

    m_addButton = new QPushButton("+ Add Card", this);
    m_addButton->setStyleSheet(
        "QPushButton {"
        "  background: qlineargradient(x1:0, y1:0, x2:1, y2:0, "
        "              stop:0 #E91E63, stop:1 #F06292);"
        "  color: white;"
        "  border: none;"
        "  border-radius: 8px;"
        "  padding: 12px 24px;"
        "  font-size: 16px;"
        "  font-weight: bold;"
        "}"
        "QPushButton:hover {"
        "  background: qlineargradient(x1:0, y1:0, x2:1, y2:0, "
        "              stop:0 #C2185B, stop:1 #E91E63);"
        "}"
        "QPushButton:pressed {"
        "  background: #AD1457;"
        "}");
    m_addButton->setFixedHeight(45);
    m_addButton->setCursor(Qt::PointingHandCursor);

    headerLayout->addWidget(m_titleLabel);
    headerLayout->addStretch();
    headerLayout->addWidget(m_addButton);

    mainLayout->addLayout(headerLayout);

    // Search bar
    m_searchInput = new QLineEdit(this);
    m_searchInput->setPlaceholderText("Search cards by nickname, issuer, or last 4 digits...");
    m_searchInput->setStyleSheet(
        "QLineEdit {"
        "  background: rgba(255, 255, 255, 0.15);"
        "  border: 2px solid rgba(255, 255, 255, 0.3);"
        "  border-radius: 8px;"
        "  padding: 12px 16px;"
        "  font-size: 14px;"
        "  color: white;"
        "}"
        "QLineEdit:focus {"
        "  border-color: #E91E63;"
        "  background: rgba(255, 255, 255, 0.2);"
        "}");
    m_searchInput->setFixedHeight(45);

    mainLayout->addWidget(m_searchInput);

    // Card list
    m_cardList = new QListWidget(this);
    m_cardList->setStyleSheet(
        "QListWidget {"
        "  background: transparent;"
        "  border: none;"
        "  outline: none;"
        "}"
        "QListWidget::item {"
        "  background: rgba(255, 255, 255, 0.1);"
        "  border: 1px solid rgba(255, 255, 255, 0.2);"
        "  border-radius: 12px;"
        "  margin-bottom: 12px;"
        "  padding: 0px;"
        "}"
        "QListWidget::item:hover {"
        "  background: rgba(255, 255, 255, 0.15);"
        "  border-color: rgba(255, 255, 255, 0.4);"
        "}"
        "QListWidget::item:selected {"
        "  background: rgba(233, 30, 99, 0.3);"
        "  border-color: #E91E63;"
        "}");
    m_cardList->setSpacing(8);
    m_cardList->setSelectionMode(QAbstractItemView::SingleSelection);

    mainLayout->addWidget(m_cardList);

    // Empty state
    m_emptyStateWidget = new QWidget(this);
    QVBoxLayout *emptyLayout = new QVBoxLayout(m_emptyStateWidget);
    emptyLayout->setAlignment(Qt::AlignCenter);

    QLabel *emptyIcon = new QLabel("💳", this);
    emptyIcon->setStyleSheet("font-size: 80px;");
    emptyIcon->setAlignment(Qt::AlignCenter);

    QLabel *emptyMessage = new QLabel("No payment cards saved", this);
    emptyMessage->setStyleSheet("font-size: 24px; font-weight: bold; color: white;");
    emptyMessage->setAlignment(Qt::AlignCenter);

    QLabel *emptySubtext = new QLabel("Add a card to link it to your subscriptions", this);
    emptySubtext->setStyleSheet("font-size: 14px; color: rgba(255, 255, 255, 0.7);");
    emptySubtext->setAlignment(Qt::AlignCenter);

    QPushButton *emptyAddButton = new QPushButton("Add Your First Card", this);
    emptyAddButton->setStyleSheet(m_addButton->styleSheet());
    emptyAddButton->setFixedHeight(50);
    emptyAddButton->setCursor(Qt::PointingHandCursor);
    connect(emptyAddButton, &QPushButton::clicked, this, &CardListView::onAddCardClicked);

    emptyLayout->addStretch();
    emptyLayout->addWidget(emptyIcon);
    emptyLayout->addSpacing(20);
    emptyLayout->addWidget(emptyMessage);
    emptyLayout->addSpacing(10);
    emptyLayout->addWidget(emptySubtext);
    emptyLayout->addSpacing(30);
    emptyLayout->addWidget(emptyAddButton, 0, Qt::AlignCenter);
    emptyLayout->addStretch();

    m_emptyStateWidget->hide();
    mainLayout->addWidget(m_emptyStateWidget);
}

void CardListView::setupConnections()
{
    connect(m_addButton, &QPushButton::clicked, this, &CardListView::onAddCardClicked);
    connect(m_cardList, &QListWidget::itemClicked, this, &CardListView::onCardItemClicked);
    connect(m_searchInput, &QLineEdit::textChanged, this, &CardListView::onSearchTextChanged);

    // Connect to model signals
    connect(m_manager, &PaymentCardManager::cardAdded,
            this, &CardListView::refreshCardList);
    connect(m_manager, &PaymentCardManager::cardUpdated,
            this, &CardListView::refreshCardList);
    connect(m_manager, &PaymentCardManager::cardDeleted,
            this, &CardListView::refreshCardList);
}

void CardListView::loadCards()
{
    m_cardList->clear();

    QList<PaymentCard> cards = m_manager->getAllCards();

    if (cards.isEmpty()) {
        showEmptyState(true);
        return;
    }

    showEmptyState(false);

    for (const PaymentCard &card : cards) {
        QWidget *itemWidget = createCardItemWidget(card);

        QListWidgetItem *item = new QListWidgetItem(m_cardList);
        item->setSizeHint(itemWidget->sizeHint());
        item->setData(Qt::UserRole, card.getId());

        m_cardList->addItem(item);
        m_cardList->setItemWidget(item, itemWidget);
    }
}

void CardListView::showEmptyState(bool show)
{
    m_cardList->setVisible(!show);
    m_searchInput->setVisible(!show);
    m_emptyStateWidget->setVisible(show);
}

QWidget *CardListView::createCardItemWidget(const PaymentCard &card)
{
    QWidget *widget = new QWidget();
    widget->setMinimumHeight(120);

    QVBoxLayout *layout = new QVBoxLayout(widget);
    layout->setContentsMargins(20, 16, 20, 16);
    layout->setSpacing(8);

    // Card nickname (bold)
    QLabel *nickname = new QLabel(card.getNickname(), widget);
    nickname->setStyleSheet("font-size: 18px; font-weight: bold; color: white;");

    // Issuer (secondary text)
    QLabel *issuer = new QLabel(card.getIssuer(), widget);
    issuer->setStyleSheet("font-size: 14px; color: rgba(255, 255, 255, 0.8);");

    // Masked card number
    QLabel *maskedNumber = new QLabel(card.getMaskedCardNumber(), widget);
    maskedNumber->setStyleSheet("font-size: 16px; color: white; letter-spacing: 2px;");

    // Expiry info with status
    QString expiryText = card.getExpiryString();
    QString expiryStatus = card.getExpiryStatus();
    QString expiryStyle = "font-size: 13px; color: rgba(255, 255, 255, 0.9);";

    if (!expiryStatus.isEmpty()) {
        expiryText += QString(" - %1").arg(expiryStatus);
        if (expiryStatus == "Expired") {
            expiryStyle = "font-size: 13px; color: #F44336; font-weight: bold;";
        } else if (expiryStatus == "Expiring Soon") {
            expiryStyle = "font-size: 13px; color: #FFC107; font-weight: bold;";
        }
    }

    QLabel *expiry = new QLabel(expiryText, widget);
    expiry->setStyleSheet(expiryStyle);

    layout->addWidget(nickname);
    layout->addWidget(issuer);
    layout->addSpacing(4);
    layout->addWidget(maskedNumber);
    layout->addWidget(expiry);

    return widget;
}

void CardListView::onAddCardClicked()
{
    CardFormDialog dialog(this);

    if (dialog.exec() == QDialog::Accepted) {
        PaymentCard newCard = dialog.getCard();
        QString cvc = dialog.getCVC();

        int cardId = m_manager->addCard(newCard, cvc);

        if (cardId > 0) {
            QMessageBox::information(this, "Success", "Payment card added successfully!");
            refreshCardList();
        } else {
            QMessageBox::critical(this, "Error", "Failed to add payment card. Please try again.");
        }
    }
}

void CardListView::onCardItemClicked(QListWidgetItem *item)
{
    int cardId = item->data(Qt::UserRole).toInt();
    PaymentCard card = m_manager->getCardById(cardId);

    if (card.getId() < 0) {
        QMessageBox::warning(this, "Error", "Card not found.");
        return;
    }

    CardFormDialog dialog(card, this);

    if (dialog.exec() == QDialog::Accepted) {
        PaymentCard updatedCard = dialog.getCard();
        QString cvc = dialog.getCVC();

        // Determine CVC action
        QString cvcAction = cvc;
        if (cvc.isEmpty() && card.hasCVC()) {
            // User left it empty, keep existing
            cvcAction = "";
        }

        if (m_manager->updateCard(cardId, updatedCard, cvcAction)) {
            QMessageBox::information(this, "Success", "Payment card updated successfully!");
            refreshCardList();
        } else {
            QMessageBox::critical(this, "Error", "Failed to update payment card. Please try again.");
        }
    }
}

void CardListView::onSearchTextChanged(const QString &text)
{
    m_model->setFilter(text);
    loadCards(); // Reload with filter
}

void CardListView::refreshCardList()
{
    loadCards();
}
