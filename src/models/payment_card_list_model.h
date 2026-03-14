#pragma once

#include <QAbstractListModel>
#include <QList>
#include "payment_card.h"

class PaymentCardManager;

/**
 * @brief Qt list model for payment cards
 *
 * Provides card data to Qt views (QListView, QListWidget).
 * Automatically updates when cards are added, updated, or deleted.
 */
class PaymentCardListModel : public QAbstractListModel
{
    Q_OBJECT

public:
    enum CardRoles {
        IdRole = Qt::UserRole + 1,
        NicknameRole,
        IssuerRole,
        Last4Role,
        MaskedNumberRole,
        ExpiryDateRole,
        ExpiryStringRole,
        ExpiryStatusRole,
        IsExpiredRole,
        IsExpiringSoonRole,
        HasCVCRole
    };

    explicit PaymentCardListModel(QObject *parent = nullptr);

    // QAbstractListModel interface
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    // Custom methods
    void refresh();
    void setFilter(const QString &searchText);
    PaymentCard getCard(int row) const;

private slots:
    void onCardAdded(int cardId);
    void onCardUpdated(int cardId);
    void onCardDeleted(int cardId);

private:
    QList<PaymentCard> m_cards;
    QList<PaymentCard> m_filteredCards;
    QString m_filterText;
    PaymentCardManager *m_manager;

    void applyFilter();
    bool matchesFilter(const PaymentCard &card) const;
};
