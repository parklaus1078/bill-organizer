#include "card_database.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QStandardPaths>
#include <QDir>
#include <QtDebug>

CardDatabase *CardDatabase::s_instance = nullptr;

CardDatabase::CardDatabase()
{
}

CardDatabase::~CardDatabase()
{
    if (m_database.isOpen()) {
        m_database.close();
    }
}

CardDatabase *CardDatabase::instance()
{
    if (!s_instance) {
        s_instance = new CardDatabase();
    }
    return s_instance;
}

bool CardDatabase::initialize()
{
    // Create database directory if it doesn't exist
    QString dataPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir dataDir(dataPath);
    if (!dataDir.exists()) {
        dataDir.mkpath(".");
    }

    // Open SQLite database
    QString dbPath = dataPath + "/billorganizer.db";
    m_database = QSqlDatabase::addDatabase("QSQLITE");
    m_database.setDatabaseName(dbPath);

    if (!m_database.open()) {
        qCritical() << "Failed to open database:" << m_database.lastError().text();
        return false;
    }

    return createTables();
}

bool CardDatabase::createTables()
{
    QSqlQuery query(m_database);

    // Create payment_cards table
    QString createTable = R"(
        CREATE TABLE IF NOT EXISTS payment_cards (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            nickname TEXT NOT NULL,
            issuer TEXT NOT NULL,
            last4_digits TEXT NOT NULL,
            expiry_month INTEGER NOT NULL,
            expiry_year INTEGER NOT NULL,
            encrypted_cvc BLOB,
            created_at TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP,
            updated_at TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP
        )
    )";

    if (!query.exec(createTable)) {
        qCritical() << "Failed to create payment_cards table:" << query.lastError().text();
        return false;
    }

    // Create index for duplicate detection
    QString createIndex = R"(
        CREATE INDEX IF NOT EXISTS idx_last4_expiry
        ON payment_cards(last4_digits, expiry_month, expiry_year)
    )";

    if (!query.exec(createIndex)) {
        qWarning() << "Failed to create index:" << query.lastError().text();
        // Non-critical, continue
    }

    return true;
}

int CardDatabase::insertCard(PaymentCard &card)
{
    QSqlQuery query(m_database);
    query.prepare(R"(
        INSERT INTO payment_cards
        (nickname, issuer, last4_digits, expiry_month, expiry_year, encrypted_cvc, created_at, updated_at)
        VALUES (:nickname, :issuer, :last4, :month, :year, :cvc, :created, :updated)
    )");

    query.bindValue(":nickname", card.getNickname());
    query.bindValue(":issuer", card.getIssuer());
    query.bindValue(":last4", card.getLast4Digits());
    query.bindValue(":month", card.getExpiryDate().month());
    query.bindValue(":year", card.getExpiryDate().year());
    query.bindValue(":cvc", card.hasCVC() ? card.getEncryptedCVC() : QVariant());

    QDateTime now = QDateTime::currentDateTime();
    query.bindValue(":created", now.toString(Qt::ISODate));
    query.bindValue(":updated", now.toString(Qt::ISODate));

    if (!query.exec()) {
        qCritical() << "Failed to insert card:" << query.lastError().text();
        return -1;
    }

    int newId = query.lastInsertId().toInt();
    card.setId(newId);
    card.setCreatedAt(now);
    card.setUpdatedAt(now);

    return newId;
}

bool CardDatabase::updateCard(const PaymentCard &card)
{
    QSqlQuery query(m_database);
    query.prepare(R"(
        UPDATE payment_cards
        SET nickname = :nickname,
            issuer = :issuer,
            last4_digits = :last4,
            expiry_month = :month,
            expiry_year = :year,
            encrypted_cvc = :cvc,
            updated_at = :updated
        WHERE id = :id
    )");

    query.bindValue(":nickname", card.getNickname());
    query.bindValue(":issuer", card.getIssuer());
    query.bindValue(":last4", card.getLast4Digits());
    query.bindValue(":month", card.getExpiryDate().month());
    query.bindValue(":year", card.getExpiryDate().year());
    query.bindValue(":cvc", card.hasCVC() ? card.getEncryptedCVC() : QVariant());
    query.bindValue(":updated", QDateTime::currentDateTime().toString(Qt::ISODate));
    query.bindValue(":id", card.getId());

    if (!query.exec()) {
        qCritical() << "Failed to update card:" << query.lastError().text();
        return false;
    }

    return query.numRowsAffected() > 0;
}

bool CardDatabase::deleteCard(int cardId)
{
    QSqlQuery query(m_database);
    query.prepare("DELETE FROM payment_cards WHERE id = :id");
    query.bindValue(":id", cardId);

    if (!query.exec()) {
        qCritical() << "Failed to delete card:" << query.lastError().text();
        return false;
    }

    return query.numRowsAffected() > 0;
}

PaymentCard CardDatabase::getCardById(int cardId)
{
    QSqlQuery query(m_database);
    query.prepare("SELECT * FROM payment_cards WHERE id = :id");
    query.bindValue(":id", cardId);

    if (!query.exec()) {
        qCritical() << "Failed to query card:" << query.lastError().text();
        return PaymentCard();
    }

    if (query.next()) {
        return cardFromQuery(query);
    }

    return PaymentCard();
}

QList<PaymentCard> CardDatabase::getAllCards()
{
    QList<PaymentCard> cards;

    QSqlQuery query(m_database);
    if (!query.exec("SELECT * FROM payment_cards ORDER BY created_at DESC")) {
        qCritical() << "Failed to query all cards:" << query.lastError().text();
        return cards;
    }

    while (query.next()) {
        cards.append(cardFromQuery(query));
    }

    return cards;
}

bool CardDatabase::isDuplicate(const QString &last4, const QDate &expiry, int excludeId)
{
    QSqlQuery query(m_database);
    query.prepare(R"(
        SELECT COUNT(*) FROM payment_cards
        WHERE last4_digits = :last4
        AND expiry_month = :month
        AND expiry_year = :year
        AND id != :excludeId
    )");

    query.bindValue(":last4", last4);
    query.bindValue(":month", expiry.month());
    query.bindValue(":year", expiry.year());
    query.bindValue(":excludeId", excludeId);

    if (!query.exec()) {
        qWarning() << "Failed to check duplicate:" << query.lastError().text();
        return false;
    }

    if (query.next()) {
        return query.value(0).toInt() > 0;
    }

    return false;
}

int CardDatabase::getSubscriptionCount(int cardId)
{
    // Placeholder for future subscription integration
    Q_UNUSED(cardId);
    return 0;
}

PaymentCard CardDatabase::cardFromQuery(QSqlQuery &query)
{
    int id = query.value("id").toInt();
    QString nickname = query.value("nickname").toString();
    QString issuer = query.value("issuer").toString();
    QString last4 = query.value("last4_digits").toString();
    int month = query.value("expiry_month").toInt();
    int year = query.value("expiry_year").toInt();
    QByteArray cvc = query.value("encrypted_cvc").toByteArray();

    QDate expiryDate(year, month, 1);

    PaymentCard card(id, nickname, issuer, last4, expiryDate, cvc);

    QString createdStr = query.value("created_at").toString();
    QString updatedStr = query.value("updated_at").toString();

    if (!createdStr.isEmpty()) {
        card.setCreatedAt(QDateTime::fromString(createdStr, Qt::ISODate));
    }
    if (!updatedStr.isEmpty()) {
        card.setUpdatedAt(QDateTime::fromString(updatedStr, Qt::ISODate));
    }

    return card;
}
