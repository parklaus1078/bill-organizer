#include "payment_card_manager.h"
#include "../utils/card_database.h"
#include "../utils/card_encryption.h"
#include <QRegularExpression>
#include <QtDebug>

PaymentCardManager *PaymentCardManager::s_instance = nullptr;

PaymentCardManager::PaymentCardManager(QObject *parent)
    : QObject(parent)
    , m_database(nullptr)
    , m_encryption(nullptr)
    , m_initialized(false)
{
}

PaymentCardManager::~PaymentCardManager()
{
}

PaymentCardManager *PaymentCardManager::instance()
{
    if (!s_instance) {
        s_instance = new PaymentCardManager();
    }
    return s_instance;
}

bool PaymentCardManager::initialize()
{
    if (m_initialized) {
        return true;
    }

    // Initialize database
    m_database = CardDatabase::instance();
    if (!m_database->initialize()) {
        emit operationFailed("Failed to initialize database");
        return false;
    }

    // Initialize encryption
    m_encryption = CardEncryption::instance();
    if (!m_encryption->initializeKey()) {
        emit operationFailed("Failed to initialize encryption");
        return false;
    }

    m_initialized = true;
    return true;
}

QList<PaymentCard> PaymentCardManager::getAllCards()
{
    if (!m_initialized) {
        qWarning() << "PaymentCardManager not initialized";
        return QList<PaymentCard>();
    }

    return m_database->getAllCards();
}

PaymentCard PaymentCardManager::getCardById(int id)
{
    if (!m_initialized) {
        qWarning() << "PaymentCardManager not initialized";
        return PaymentCard();
    }

    return m_database->getCardById(id);
}

int PaymentCardManager::addCard(PaymentCard &card, const QString &plainCVC)
{
    if (!m_initialized) {
        emit operationFailed("Manager not initialized");
        return -1;
    }

    // Validate card data
    ValidationResult validation = validateCard(card, plainCVC);
    if (!validation.isValid) {
        qWarning() << "Card validation failed:" << validation.errors;
        emit operationFailed("Validation failed");
        return -1;
    }

    // Encrypt CVC if provided
    if (!plainCVC.isEmpty()) {
        QByteArray encryptedCVC = m_encryption->encrypt(plainCVC);
        card.setEncryptedCVC(encryptedCVC);
    }

    // Insert into database
    int newId = m_database->insertCard(card);
    if (newId > 0) {
        emit cardAdded(newId);
        return newId;
    }

    emit operationFailed("Failed to save card");
    return -1;
}

bool PaymentCardManager::updateCard(int id, PaymentCard &updatedCard, const QString &plainCVC)
{
    if (!m_initialized) {
        emit operationFailed("Manager not initialized");
        return false;
    }

    // Get existing card
    PaymentCard existingCard = m_database->getCardById(id);
    if (existingCard.getId() < 0) {
        emit operationFailed("Card not found");
        return false;
    }

    // Set the ID
    updatedCard.setId(id);

    // Validate updated data
    ValidationResult validation = validateCard(updatedCard, plainCVC);
    if (!validation.isValid) {
        qWarning() << "Card validation failed:" << validation.errors;
        emit operationFailed("Validation failed");
        return false;
    }

    // Handle CVC update
    if (plainCVC == "CLEAR") {
        // User wants to clear CVC
        updatedCard.clearCVC();
    } else if (!plainCVC.isEmpty()) {
        // User provided new CVC
        QByteArray encryptedCVC = m_encryption->encrypt(plainCVC);
        updatedCard.setEncryptedCVC(encryptedCVC);
    } else {
        // Keep existing CVC
        updatedCard.setEncryptedCVC(existingCard.getEncryptedCVC());
    }

    // Update in database
    if (m_database->updateCard(updatedCard)) {
        emit cardUpdated(id);
        return true;
    }

    emit operationFailed("Failed to update card");
    return false;
}

bool PaymentCardManager::deleteCard(int id)
{
    if (!m_initialized) {
        emit operationFailed("Manager not initialized");
        return false;
    }

    if (m_database->deleteCard(id)) {
        emit cardDeleted(id);
        return true;
    }

    emit operationFailed("Failed to delete card");
    return false;
}

ValidationResult PaymentCardManager::validateCard(const PaymentCard &card, const QString &cvc)
{
    ValidationResult result;

    // Validate nickname
    QString nickname = card.getNickname().simplified();
    if (nickname.isEmpty()) {
        result.isValid = false;
        result.errors["nickname"] = "Card nickname is required";
    } else if (nickname.length() > 50) {
        result.isValid = false;
        result.errors["nickname"] = "Card nickname must be 50 characters or less";
    }

    // Validate issuer
    QString issuer = card.getIssuer().simplified();
    if (issuer.isEmpty()) {
        result.isValid = false;
        result.errors["issuer"] = "Issuer/Bank is required";
    } else if (issuer.length() > 50) {
        result.isValid = false;
        result.errors["issuer"] = "Issuer/Bank must be 50 characters or less";
    }

    // Validate last 4 digits
    QString last4 = card.getLast4Digits();
    QRegularExpression last4Regex("^[0-9]{4}$");
    if (!last4Regex.match(last4).hasMatch()) {
        result.isValid = false;
        result.errors["last4"] = "Last 4 digits must be exactly 4 numeric digits";
    }

    // Validate expiry date
    QDate expiry = card.getExpiryDate();
    if (!expiry.isValid()) {
        result.isValid = false;
        result.errors["expiry"] = "Expiry date is required";
    } else {
        // Check if expiry is in the future
        QDate today = QDate::currentDate();
        QDate cardExpiry(expiry.year(), expiry.month(), expiry.daysInMonth());
        if (cardExpiry < today) {
            result.isValid = false;
            result.errors["expiry"] = "Expiry date must be in the future";
        }
    }

    // Validate CVC (optional)
    if (!cvc.isEmpty() && cvc != "CLEAR") {
        QRegularExpression cvcRegex("^[0-9]{3,4}$");
        if (!cvcRegex.match(cvc).hasMatch()) {
            result.isValid = false;
            result.errors["cvc"] = "CVC must be 3-4 numeric digits";
        }
    }

    return result;
}

bool PaymentCardManager::isDuplicate(const QString &last4, const QDate &expiry, int excludeId)
{
    if (!m_initialized) {
        return false;
    }

    return m_database->isDuplicate(last4, expiry, excludeId);
}

QList<PaymentCard> PaymentCardManager::getActiveCards()
{
    QList<PaymentCard> allCards = getAllCards();
    QList<PaymentCard> activeCards;

    for (const PaymentCard &card : allCards) {
        if (!card.isExpired()) {
            activeCards.append(card);
        }
    }

    return activeCards;
}

QList<PaymentCard> PaymentCardManager::getExpiredCards()
{
    QList<PaymentCard> allCards = getAllCards();
    QList<PaymentCard> expiredCards;

    for (const PaymentCard &card : allCards) {
        if (card.isExpired()) {
            expiredCards.append(card);
        }
    }

    return expiredCards;
}

int PaymentCardManager::getSubscriptionCount(int cardId)
{
    if (!m_initialized) {
        return 0;
    }

    return m_database->getSubscriptionCount(cardId);
}

QString PaymentCardManager::getMaskedCVC(const PaymentCard &card)
{
    if (!card.hasCVC()) {
        return "";
    }
    return "•••";
}
