#include "payment_card_list_model.h"
#include "payment_card_manager.h"

PaymentCardListModel::PaymentCardListModel(QObject *parent)
    : QAbstractListModel(parent)
    , m_manager(PaymentCardManager::instance())
{
    // Connect to manager signals
    connect(m_manager, &PaymentCardManager::cardAdded,
            this, &PaymentCardListModel::onCardAdded);
    connect(m_manager, &PaymentCardManager::cardUpdated,
            this, &PaymentCardListModel::onCardUpdated);
    connect(m_manager, &PaymentCardManager::cardDeleted,
            this, &PaymentCardListModel::onCardDeleted);

    // Load initial data
    refresh();
}

int PaymentCardListModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid()) {
        return 0;
    }

    return m_filteredCards.size();
}

QVariant PaymentCardListModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= m_filteredCards.size()) {
        return QVariant();
    }

    const PaymentCard &card = m_filteredCards.at(index.row());

    switch (role) {
    case IdRole:
        return card.getId();
    case NicknameRole:
    case Qt::DisplayRole:
        return card.getNickname();
    case IssuerRole:
        return card.getIssuer();
    case Last4Role:
        return card.getLast4Digits();
    case MaskedNumberRole:
        return card.getMaskedCardNumber();
    case ExpiryDateRole:
        return card.getExpiryDate();
    case ExpiryStringRole:
        return card.getExpiryString();
    case ExpiryStatusRole:
        return card.getExpiryStatus();
    case IsExpiredRole:
        return card.isExpired();
    case IsExpiringSoonRole:
        return card.isExpiringSoon();
    case HasCVCRole:
        return card.hasCVC();
    default:
        return QVariant();
    }
}

QHash<int, QByteArray> PaymentCardListModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[IdRole] = "cardId";
    roles[NicknameRole] = "nickname";
    roles[IssuerRole] = "issuer";
    roles[Last4Role] = "last4";
    roles[MaskedNumberRole] = "maskedNumber";
    roles[ExpiryDateRole] = "expiryDate";
    roles[ExpiryStringRole] = "expiryString";
    roles[ExpiryStatusRole] = "expiryStatus";
    roles[IsExpiredRole] = "isExpired";
    roles[IsExpiringSoonRole] = "isExpiringSoon";
    roles[HasCVCRole] = "hasCVC";
    return roles;
}

void PaymentCardListModel::refresh()
{
    beginResetModel();
    m_cards = m_manager->getAllCards();
    applyFilter();
    endResetModel();
}

void PaymentCardListModel::setFilter(const QString &searchText)
{
    m_filterText = searchText.simplified();
    beginResetModel();
    applyFilter();
    endResetModel();
}

PaymentCard PaymentCardListModel::getCard(int row) const
{
    if (row >= 0 && row < m_filteredCards.size()) {
        return m_filteredCards.at(row);
    }
    return PaymentCard();
}

void PaymentCardListModel::onCardAdded(int cardId)
{
    Q_UNUSED(cardId);
    refresh();
}

void PaymentCardListModel::onCardUpdated(int cardId)
{
    // Find and update the specific card
    for (int i = 0; i < m_filteredCards.size(); ++i) {
        if (m_filteredCards[i].getId() == cardId) {
            PaymentCard updatedCard = m_manager->getCardById(cardId);
            m_filteredCards[i] = updatedCard;

            // Also update in main list
            for (int j = 0; j < m_cards.size(); ++j) {
                if (m_cards[j].getId() == cardId) {
                    m_cards[j] = updatedCard;
                    break;
                }
            }

            QModelIndex topLeft = index(i, 0);
            QModelIndex bottomRight = index(i, 0);
            emit dataChanged(topLeft, bottomRight);
            return;
        }
    }

    // Card not in filtered list, refresh to be safe
    refresh();
}

void PaymentCardListModel::onCardDeleted(int cardId)
{
    // Find and remove the card
    for (int i = 0; i < m_filteredCards.size(); ++i) {
        if (m_filteredCards[i].getId() == cardId) {
            beginRemoveRows(QModelIndex(), i, i);
            m_filteredCards.removeAt(i);

            // Also remove from main list
            for (int j = 0; j < m_cards.size(); ++j) {
                if (m_cards[j].getId() == cardId) {
                    m_cards.removeAt(j);
                    break;
                }
            }

            endRemoveRows();
            return;
        }
    }
}

void PaymentCardListModel::applyFilter()
{
    m_filteredCards.clear();

    if (m_filterText.isEmpty()) {
        m_filteredCards = m_cards;
    } else {
        for (const PaymentCard &card : m_cards) {
            if (matchesFilter(card)) {
                m_filteredCards.append(card);
            }
        }
    }
}

bool PaymentCardListModel::matchesFilter(const PaymentCard &card) const
{
    if (m_filterText.isEmpty()) {
        return true;
    }

    QString filter = m_filterText.toLower();
    return card.getNickname().toLower().contains(filter) ||
           card.getIssuer().toLower().contains(filter) ||
           card.getLast4Digits().contains(filter);
}
