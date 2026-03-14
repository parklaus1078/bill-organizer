#pragma once

#include <QWidget>
#include <QListWidget>
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>
#include "../models/payment_card.h"

class PaymentCardManager;
class PaymentCardListModel;

/**
 * @brief Main view for displaying and managing payment cards
 *
 * Shows a list of payment cards with options to add, edit, and delete.
 * Follows the purple gradient design from the UI reference.
 */
class CardListView : public QWidget
{
    Q_OBJECT

public:
    explicit CardListView(QWidget *parent = nullptr);

private slots:
    void onAddCardClicked();
    void onCardItemClicked(QListWidgetItem *item);
    void onSearchTextChanged(const QString &text);
    void refreshCardList();

private:
    void setupUI();
    void setupConnections();
    void loadCards();
    void showEmptyState(bool show);
    QWidget *createCardItemWidget(const PaymentCard &card);

    // UI Components
    QListWidget *m_cardList;
    QPushButton *m_addButton;
    QLineEdit *m_searchInput;
    QWidget *m_emptyStateWidget;
    QLabel *m_titleLabel;

    PaymentCardManager *m_manager;
    PaymentCardListModel *m_model;
};
