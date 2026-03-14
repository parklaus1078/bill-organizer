#include <QtTest/QtTest>
#include "../../src/models/payment_card.h"
#include "../test_helpers.h"

/**
 * @brief Unit tests for PaymentCard model
 *
 * Tests individual PaymentCard functionality including:
 * - Getters and setters
 * - Derived properties (getMaskedCardNumber, isExpired, isExpiringSoon, getExpiryStatus)
 * - CVC management
 */
class TestPaymentCard : public QObject
{
    Q_OBJECT

private slots:
    void init();
    void cleanup();

    // TC-UNIT-001 to TC-UNIT-006: Getters and Setters
    void testSetAndGetNickname();
    void testSetAndGetIssuer();
    void testSetAndGetLast4Digits();
    void testSetAndGetExpiryDate();
    void testSetAndGetEncryptedCVC();
    void testClearCVC();

    // TC-UNIT-010 to TC-UNIT-012: getMaskedCardNumber()
    void testGetMaskedCardNumber_1234();
    void testGetMaskedCardNumber_0000();
    void testGetMaskedCardNumber_9999();

    // TC-UNIT-020 to TC-UNIT-023: isExpired()
    void testIsExpired_futureDate();
    void testIsExpired_pastDate();
    void testIsExpired_currentMonth();
    void testIsExpired_nextMonth();

    // TC-UNIT-030 to TC-UNIT-034: isExpiringSoon()
    void testIsExpiringSoon_29days();
    void testIsExpiringSoon_30days();
    void testIsExpiringSoon_31days();
    void testIsExpiringSoon_90days();
    void testIsExpiringSoon_alreadyExpired();

    // TC-UNIT-040 to TC-UNIT-042: getExpiryStatus()
    void testGetExpiryStatus_expired();
    void testGetExpiryStatus_expiringSoon();
    void testGetExpiryStatus_active();

    // TC-UNIT-300 to TC-UNIT-302: Memory Management
    void testCopyConstructor();
    void testAssignmentOperator();

    // TC-UNIT-320 to TC-UNIT-322: Null and Empty Handling
    void testNullQString();
    void testNullQDate();
    void testEmptyQByteArray();

private:
    PaymentCard *card;
};

void TestPaymentCard::init()
{
    card = new PaymentCard();
}

void TestPaymentCard::cleanup()
{
    delete card;
    card = nullptr;
}

// TC-UNIT-001: Set and get nickname
void TestPaymentCard::testSetAndGetNickname()
{
    card->setNickname("My Visa");
    QCOMPARE(card->getNickname(), QString("My Visa"));
}

// TC-UNIT-002: Set and get issuer
void TestPaymentCard::testSetAndGetIssuer()
{
    card->setIssuer("Chase");
    QCOMPARE(card->getIssuer(), QString("Chase"));
}

// TC-UNIT-003: Set and get last 4 digits
void TestPaymentCard::testSetAndGetLast4Digits()
{
    card->setLast4Digits("1234");
    QCOMPARE(card->getLast4Digits(), QString("1234"));
}

// TC-UNIT-004: Set and get expiry date
void TestPaymentCard::testSetAndGetExpiryDate()
{
    QDate testDate(2028, 12, 1);
    card->setExpiryDate(testDate);
    QCOMPARE(card->getExpiryDate(), testDate);
}

// TC-UNIT-005: Set and get encrypted CVC
void TestPaymentCard::testSetAndGetEncryptedCVC()
{
    QByteArray testCVC = QByteArray::fromHex("abc123");
    card->setEncryptedCVC(testCVC);
    QVERIFY(card->hasCVC());
    QCOMPARE(card->getEncryptedCVC(), testCVC);
}

// TC-UNIT-006: Clear CVC
void TestPaymentCard::testClearCVC()
{
    card->setEncryptedCVC(QByteArray::fromHex("abc123"));
    QVERIFY(card->hasCVC());

    card->clearCVC();
    QVERIFY(!card->hasCVC());
}

// TC-UNIT-010: Mask card with last 4 digits "1234"
void TestPaymentCard::testGetMaskedCardNumber_1234()
{
    card->setLast4Digits("1234");
    QCOMPARE(card->getMaskedCardNumber(), QString("•••• •••• •••• 1234"));
}

// TC-UNIT-011: Mask card with last 4 digits "0000"
void TestPaymentCard::testGetMaskedCardNumber_0000()
{
    card->setLast4Digits("0000");
    QCOMPARE(card->getMaskedCardNumber(), QString("•••• •••• •••• 0000"));
}

// TC-UNIT-012: Mask card with last 4 digits "9999"
void TestPaymentCard::testGetMaskedCardNumber_9999()
{
    card->setLast4Digits("9999");
    QCOMPARE(card->getMaskedCardNumber(), QString("•••• •••• •••• 9999"));
}

// TC-UNIT-020: Card expiring in future
void TestPaymentCard::testIsExpired_futureDate()
{
    card->setExpiryDate(QDate(2030, 12, 1));
    QVERIFY(!card->isExpired());
}

// TC-UNIT-021: Card expired in past
void TestPaymentCard::testIsExpired_pastDate()
{
    card->setExpiryDate(QDate(2025, 1, 1));
    QVERIFY(card->isExpired());
}

// TC-UNIT-022: Card expiring current month
void TestPaymentCard::testIsExpired_currentMonth()
{
    QDate currentDate = QDate::currentDate();
    card->setExpiryDate(QDate(currentDate.year(), currentDate.month(), 1));
    QVERIFY(!card->isExpired());
}

// TC-UNIT-023: Card expiring next month
void TestPaymentCard::testIsExpired_nextMonth()
{
    QDate nextMonth = QDate::currentDate().addMonths(1);
    card->setExpiryDate(QDate(nextMonth.year(), nextMonth.month(), 1));
    QVERIFY(!card->isExpired());
}

// TC-UNIT-030: Card expiring in 29 days
void TestPaymentCard::testIsExpiringSoon_29days()
{
    // Set expiry to current month (will use last day of month)
    QDate today = QDate::currentDate();
    card->setExpiryDate(QDate(today.year(), today.month(), 1));
    QVERIFY(card->isExpiringSoon());
}

// TC-UNIT-031: Card expiring in 30 days
void TestPaymentCard::testIsExpiringSoon_30days()
{
    // Set expiry to next month (within 30 days for most cases)
    QDate today = QDate::currentDate();
    card->setExpiryDate(QDate(today.year(), today.month(), 1).addMonths(1));

    // Calculate actual days until end of next month
    QDate nextMonthEnd(today.year(), today.addMonths(1).month(),
                       today.addMonths(1).daysInMonth());
    int days = today.daysTo(nextMonthEnd);

    if (days <= 30) {
        QVERIFY(card->isExpiringSoon());
    } else {
        QVERIFY(!card->isExpiringSoon());
    }
}

// TC-UNIT-032: Card expiring in 31 days
void TestPaymentCard::testIsExpiringSoon_31days()
{
    // Set expiry to 2 months from now
    QDate today = QDate::currentDate();
    card->setExpiryDate(today.addMonths(2));
    QVERIFY(!card->isExpiringSoon());
}

// TC-UNIT-033: Card expiring in 90 days
void TestPaymentCard::testIsExpiringSoon_90days()
{
    QDate today = QDate::currentDate();
    card->setExpiryDate(today.addMonths(3));
    QVERIFY(!card->isExpiringSoon());
}

// TC-UNIT-034: Card already expired
void TestPaymentCard::testIsExpiringSoon_alreadyExpired()
{
    QDate today = QDate::currentDate();
    card->setExpiryDate(today.addMonths(-1));
    QVERIFY(!card->isExpiringSoon());
}

// TC-UNIT-040: Card expired
void TestPaymentCard::testGetExpiryStatus_expired()
{
    QDate today = QDate::currentDate();
    card->setExpiryDate(today.addMonths(-1));
    QCOMPARE(card->getExpiryStatus(), QString("Expired"));
}

// TC-UNIT-041: Card expiring within 30 days
void TestPaymentCard::testGetExpiryStatus_expiringSoon()
{
    QDate today = QDate::currentDate();
    // Set to current month - will expire at end of current month
    card->setExpiryDate(QDate(today.year(), today.month(), 1));
    QCOMPARE(card->getExpiryStatus(), QString("Expiring Soon"));
}

// TC-UNIT-042: Card valid and not expiring soon
void TestPaymentCard::testGetExpiryStatus_active()
{
    card->setExpiryDate(QDate::currentDate().addDays(60));
    QCOMPARE(card->getExpiryStatus(), QString(""));
}

// TC-UNIT-301: PaymentCard copy constructor
void TestPaymentCard::testCopyConstructor()
{
    card->setNickname("Original");
    card->setIssuer("Bank A");
    card->setLast4Digits("1234");

    PaymentCard copy(*card);

    QCOMPARE(copy.getNickname(), card->getNickname());
    QCOMPARE(copy.getIssuer(), card->getIssuer());
    QCOMPARE(copy.getLast4Digits(), card->getLast4Digits());
}

// TC-UNIT-302: PaymentCard assignment operator
void TestPaymentCard::testAssignmentOperator()
{
    card->setNickname("Original");
    card->setIssuer("Bank A");

    PaymentCard assigned;
    assigned = *card;

    QCOMPARE(assigned.getNickname(), card->getNickname());
    QCOMPARE(assigned.getIssuer(), card->getIssuer());
}

// TC-UNIT-320: Null QString for nickname
void TestPaymentCard::testNullQString()
{
    card->setNickname(QString());
    QVERIFY(card->getNickname().isEmpty());
}

// TC-UNIT-321: Null QDate for expiry
void TestPaymentCard::testNullQDate()
{
    card->setExpiryDate(QDate());
    QVERIFY(!card->getExpiryDate().isValid());
}

// TC-UNIT-322: Empty QByteArray for encrypted CVC
void TestPaymentCard::testEmptyQByteArray()
{
    card->setEncryptedCVC(QByteArray());
    QVERIFY(!card->hasCVC());
}

QTEST_MAIN(TestPaymentCard)
#include "tst_payment_card.moc"
