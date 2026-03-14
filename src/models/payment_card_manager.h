#pragma once

#include <QObject>
#include <QString>
#include <QList>
#include <QMap>
#include "payment_card.h"

class CardDatabase;
class CardEncryption;

/**
 * @brief Validation result for payment card data
 */
struct ValidationResult
{
    bool isValid;
    QMap<QString, QString> errors;

    ValidationResult() : isValid(true) {}
};

/**
 * @brief Manager for payment card operations
 *
 * PaymentCardManager is a singleton that coordinates all card-related
 * operations including CRUD, validation, encryption, and database access.
 * It emits signals when cards are added, updated, or deleted.
 */
class PaymentCardManager : public QObject
{
    Q_OBJECT

public:
    static PaymentCardManager *instance();

    /**
     * @brief Initializes the manager (database and encryption)
     * @return true if initialization successful
     */
    bool initialize();

    // CRUD operations
    QList<PaymentCard> getAllCards();
    PaymentCard getCardById(int id);

    /**
     * @brief Adds a new payment card
     * @param card The card to add
     * @param plainCVC Optional plain-text CVC (will be encrypted)
     * @return The new card ID, or -1 on failure
     */
    int addCard(PaymentCard &card, const QString &plainCVC = QString());

    /**
     * @brief Updates an existing payment card
     * @param id Card ID to update
     * @param updatedCard Card with new data
     * @param plainCVC Optional plain-text CVC (empty = keep existing, "CLEAR" = remove)
     * @return true if update successful
     */
    bool updateCard(int id, PaymentCard &updatedCard, const QString &plainCVC = QString());

    /**
     * @brief Deletes a payment card
     * @param id Card ID to delete
     * @return true if deletion successful
     */
    bool deleteCard(int id);

    // Validation
    ValidationResult validateCard(const PaymentCard &card, const QString &cvc = QString());
    bool isDuplicate(const QString &last4, const QDate &expiry, int excludeId = -1);

    // Query operations
    QList<PaymentCard> getActiveCards();
    QList<PaymentCard> getExpiredCards();
    int getSubscriptionCount(int cardId);

    /**
     * @brief Decrypts CVC for display (returns "•••")
     * @param card The card with encrypted CVC
     * @return Masked CVC string
     */
    QString getMaskedCVC(const PaymentCard &card);

signals:
    void cardAdded(int cardId);
    void cardUpdated(int cardId);
    void cardDeleted(int cardId);
    void cardsLoaded();
    void operationFailed(const QString &error);

private:
    PaymentCardManager(QObject *parent = nullptr);
    ~PaymentCardManager();

    static PaymentCardManager *s_instance;

    CardDatabase *m_database;
    CardEncryption *m_encryption;
    bool m_initialized;
};
