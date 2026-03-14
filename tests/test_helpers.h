#pragma once

#include <QDate>
#include <QString>
#include "../src/models/payment_card.h"

/**
 * @brief Test helper functions and fixtures for payment card tests
 */
namespace TestHelpers {

/**
 * @brief Creates a valid test card with future expiry date
 */
inline PaymentCard createValidCard()
{
    PaymentCard card;
    card.setNickname("Test Card");
    card.setIssuer("Test Bank");
    card.setLast4Digits("1234");
    card.setExpiryDate(QDate::currentDate().addMonths(12));
    return card;
}

/**
 * @brief Creates an expired test card
 */
inline PaymentCard createExpiredCard()
{
    PaymentCard card;
    card.setNickname("Expired Card");
    card.setIssuer("Old Bank");
    card.setLast4Digits("5678");
    card.setExpiryDate(QDate(2025, 1, 1));
    return card;
}

/**
 * @brief Creates a card expiring soon (within 30 days)
 */
inline PaymentCard createExpiringSoonCard()
{
    PaymentCard card;
    card.setNickname("Expiring Card");
    card.setIssuer("Almost Bank");
    card.setLast4Digits("9999");
    card.setExpiryDate(QDate::currentDate().addDays(20));
    return card;
}

/**
 * @brief Creates a card with encrypted CVC
 */
inline PaymentCard createCardWithCVC()
{
    PaymentCard card = createValidCard();
    // Note: Actual encryption should be done via CardEncryption
    card.setEncryptedCVC(QByteArray::fromHex("abcdef1234567890"));
    return card;
}

} // namespace TestHelpers
