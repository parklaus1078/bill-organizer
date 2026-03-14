#include "payment_card.h"
#include <QDate>

PaymentCard::PaymentCard()
    : m_id(-1)
    , m_createdAt(QDateTime::currentDateTime())
    , m_updatedAt(QDateTime::currentDateTime())
{
}

PaymentCard::PaymentCard(int id, const QString &nickname, const QString &issuer,
                         const QString &last4Digits, const QDate &expiryDate,
                         const QByteArray &encryptedCVC)
    : m_id(id)
    , m_nickname(nickname)
    , m_issuer(issuer)
    , m_last4Digits(last4Digits)
    , m_expiryDate(expiryDate)
    , m_encryptedCVC(encryptedCVC)
    , m_createdAt(QDateTime::currentDateTime())
    , m_updatedAt(QDateTime::currentDateTime())
{
}

QString PaymentCard::getMaskedCardNumber() const
{
    if (m_last4Digits.isEmpty()) {
        return "•••• •••• •••• ••••";
    }
    return QString("•••• •••• •••• %1").arg(m_last4Digits);
}

bool PaymentCard::isExpired() const
{
    if (!m_expiryDate.isValid()) {
        return false;
    }

    QDate today = QDate::currentDate();
    // Card expires at the end of the expiry month
    QDate cardExpiry(m_expiryDate.year(), m_expiryDate.month(),
                     m_expiryDate.daysInMonth());

    return cardExpiry < today;
}

bool PaymentCard::isExpiringSoon() const
{
    if (!m_expiryDate.isValid() || isExpired()) {
        return false;
    }

    QDate today = QDate::currentDate();
    QDate cardExpiry(m_expiryDate.year(), m_expiryDate.month(),
                     m_expiryDate.daysInMonth());

    // Expiring soon = within 30 days
    int daysUntilExpiry = today.daysTo(cardExpiry);
    return daysUntilExpiry >= 0 && daysUntilExpiry <= 30;
}

QString PaymentCard::getExpiryStatus() const
{
    if (isExpired()) {
        return "Expired";
    }
    if (isExpiringSoon()) {
        return "Expiring Soon";
    }
    return "";
}

QString PaymentCard::getExpiryString() const
{
    if (!m_expiryDate.isValid()) {
        return "";
    }
    return QString("Expires %1/%2")
        .arg(m_expiryDate.month(), 2, 10, QChar('0'))
        .arg(m_expiryDate.year() % 100, 2, 10, QChar('0'));
}
