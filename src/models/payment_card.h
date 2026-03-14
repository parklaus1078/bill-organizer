#pragma once

#include <QString>
#include <QDate>
#include <QDateTime>
#include <QByteArray>

/**
 * @brief Data model representing a payment card
 *
 * PaymentCard stores card information including nickname, issuer,
 * last 4 digits, expiry date, and optionally encrypted CVC.
 * This class provides derived properties for masked display and
 * expiry status checking.
 */
class PaymentCard
{
public:
    PaymentCard();
    PaymentCard(int id, const QString &nickname, const QString &issuer,
                const QString &last4Digits, const QDate &expiryDate,
                const QByteArray &encryptedCVC = QByteArray());

    // Getters
    int getId() const { return m_id; }
    QString getNickname() const { return m_nickname; }
    QString getIssuer() const { return m_issuer; }
    QString getLast4Digits() const { return m_last4Digits; }
    QDate getExpiryDate() const { return m_expiryDate; }
    QByteArray getEncryptedCVC() const { return m_encryptedCVC; }
    QDateTime getCreatedAt() const { return m_createdAt; }
    QDateTime getUpdatedAt() const { return m_updatedAt; }

    // Setters
    void setId(int id) { m_id = id; }
    void setNickname(const QString &nickname) { m_nickname = nickname; }
    void setIssuer(const QString &issuer) { m_issuer = issuer; }
    void setLast4Digits(const QString &last4) { m_last4Digits = last4; }
    void setExpiryDate(const QDate &date) { m_expiryDate = date; }
    void setEncryptedCVC(const QByteArray &cvc) { m_encryptedCVC = cvc; }
    void setCreatedAt(const QDateTime &dateTime) { m_createdAt = dateTime; }
    void setUpdatedAt(const QDateTime &dateTime) { m_updatedAt = dateTime; }
    void clearCVC() { m_encryptedCVC.clear(); }

    // Derived properties
    bool hasCVC() const { return !m_encryptedCVC.isEmpty(); }
    QString getMaskedCardNumber() const;
    bool isExpired() const;
    bool isExpiringSoon() const;
    QString getExpiryStatus() const;
    QString getExpiryString() const;

private:
    int m_id;
    QString m_nickname;
    QString m_issuer;
    QString m_last4Digits;
    QDate m_expiryDate;
    QByteArray m_encryptedCVC;
    QDateTime m_createdAt;
    QDateTime m_updatedAt;
};
