#pragma once

#include <QString>
#include <QList>
#include <QSqlDatabase>
#include "../models/payment_card.h"

/**
 * @brief Handles SQLite database operations for payment cards
 *
 * CardDatabase manages the payment_cards table, providing CRUD operations
 * and ensuring data integrity through parameterized queries.
 */
class CardDatabase
{
public:
    static CardDatabase *instance();

    /**
     * @brief Initializes the database and creates tables if needed
     * @return true if initialization successful
     */
    bool initialize();

    /**
     * @brief Inserts a new payment card into the database
     * @param card The card to insert (id will be set after insertion)
     * @return The new card ID, or -1 on failure
     */
    int insertCard(PaymentCard &card);

    /**
     * @brief Updates an existing payment card
     * @param card The card with updated data
     * @return true if update successful
     */
    bool updateCard(const PaymentCard &card);

    /**
     * @brief Deletes a payment card by ID
     * @param cardId The ID of the card to delete
     * @return true if deletion successful
     */
    bool deleteCard(int cardId);

    /**
     * @brief Retrieves a payment card by ID
     * @param cardId The ID of the card to retrieve
     * @return The payment card, or invalid card if not found
     */
    PaymentCard getCardById(int cardId);

    /**
     * @brief Retrieves all payment cards
     * @return List of all payment cards
     */
    QList<PaymentCard> getAllCards();

    /**
     * @brief Checks if a card with same last 4 digits and expiry exists
     * @param last4 Last 4 digits
     * @param expiry Expiry date
     * @param excludeId Card ID to exclude from check (for updates)
     * @return true if duplicate exists
     */
    bool isDuplicate(const QString &last4, const QDate &expiry, int excludeId = -1);

    /**
     * @brief Gets count of subscriptions using this card
     * @param cardId The card ID
     * @return Number of subscriptions (0 for now, placeholder for future)
     */
    int getSubscriptionCount(int cardId);

private:
    CardDatabase();
    ~CardDatabase();

    static CardDatabase *s_instance;
    QSqlDatabase m_database;

    bool createTables();
    PaymentCard cardFromQuery(class QSqlQuery &query);
};
