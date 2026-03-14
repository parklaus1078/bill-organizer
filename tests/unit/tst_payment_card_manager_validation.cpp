#include <QtTest/QtTest>
#include "../../src/models/payment_card_manager.h"
#include "../../src/models/payment_card.h"
#include "../test_helpers.h"

/**
 * @brief Unit tests for PaymentCardManager validation logic
 *
 * Tests all validation rules for payment card fields including:
 * - Nickname validation
 * - Issuer validation
 * - Last 4 digits validation
 * - Expiry date validation
 * - CVC validation
 * - Complete card validation
 * - Duplicate detection
 */
class TestPaymentCardManagerValidation : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();

    // TC-UNIT-100 to TC-UNIT-105: Validate Nickname
    void testValidateNickname_valid();
    void testValidateNickname_exactly50chars();
    void testValidateNickname_empty();
    void testValidateNickname_exceeds50chars();
    void testValidateNickname_withEmojis();
    void testValidateNickname_spacesOnly();

    // TC-UNIT-110 to TC-UNIT-114: Validate Issuer
    void testValidateIssuer_valid();
    void testValidateIssuer_exactly50chars();
    void testValidateIssuer_empty();
    void testValidateIssuer_exceeds50chars();
    void testValidateIssuer_spacesOnly();

    // TC-UNIT-120 to TC-UNIT-128: Validate Last 4 Digits
    void testValidateLast4_valid1234();
    void testValidateLast4_valid0000();
    void testValidateLast4_valid9999();
    void testValidateLast4_empty();
    void testValidateLast4_threeDigits();
    void testValidateLast4_fiveDigits();
    void testValidateLast4_withLetters();
    void testValidateLast4_withSpecialChars();
    void testValidateLast4_withSpaces();

    // TC-UNIT-130 to TC-UNIT-136: Validate Expiry Date
    void testValidateExpiry_futureDate();
    void testValidateExpiry_nextMonth();
    void testValidateExpiry_currentMonth();
    void testValidateExpiry_pastDate();
    void testValidateExpiry_invalidMonth0();
    void testValidateExpiry_invalidMonth13();
    void testValidateExpiry_nullDate();

    // TC-UNIT-140 to TC-UNIT-146: Validate CVC (Optional)
    void testValidateCVC_empty();
    void testValidateCVC_valid3digit();
    void testValidateCVC_valid4digit();
    void testValidateCVC_invalid2digit();
    void testValidateCVC_invalid5digit();
    void testValidateCVC_withLetters();
    void testValidateCVC_withSpecialChars();

    // TC-UNIT-150 to TC-UNIT-153: Complete Card Validation
    void testValidateCard_allValidWithoutCVC();
    void testValidateCard_allValidWithCVC();
    void testValidateCard_multipleInvalidFields();
    void testValidateCard_oneInvalidField();

private:
    PaymentCardManager *manager;
};

void TestPaymentCardManagerValidation::initTestCase()
{
    manager = PaymentCardManager::instance();
    manager->initialize();
}

void TestPaymentCardManagerValidation::cleanupTestCase()
{
    // Singleton cleanup handled automatically
}

// TC-UNIT-100: Valid nickname "My Visa"
void TestPaymentCardManagerValidation::testValidateNickname_valid()
{
    PaymentCard card = TestHelpers::createValidCard();
    card.setNickname("My Visa");

    ValidationResult result = manager->validateCard(card);
    QVERIFY(result.isValid);
    QVERIFY(!result.errors.contains("nickname"));
}

// TC-UNIT-101: Valid nickname with 50 characters
void TestPaymentCardManagerValidation::testValidateNickname_exactly50chars()
{
    PaymentCard card = TestHelpers::createValidCard();
    card.setNickname(QString("A").repeated(50));

    ValidationResult result = manager->validateCard(card);
    QVERIFY(result.isValid);
}

// TC-UNIT-102: Empty nickname
void TestPaymentCardManagerValidation::testValidateNickname_empty()
{
    PaymentCard card = TestHelpers::createValidCard();
    card.setNickname("");

    ValidationResult result = manager->validateCard(card);
    QVERIFY(!result.isValid);
    QVERIFY(result.errors.contains("nickname"));
    QVERIFY(result.errors["nickname"].contains("required"));
}

// TC-UNIT-103: Nickname exceeds 50 characters
void TestPaymentCardManagerValidation::testValidateNickname_exceeds50chars()
{
    PaymentCard card = TestHelpers::createValidCard();
    card.setNickname(QString("A").repeated(51));

    ValidationResult result = manager->validateCard(card);
    QVERIFY(!result.isValid);
    QVERIFY(result.errors.contains("nickname"));
    QVERIFY(result.errors["nickname"].contains("50 characters"));
}

// TC-UNIT-104: Nickname with emojis
void TestPaymentCardManagerValidation::testValidateNickname_withEmojis()
{
    PaymentCard card = TestHelpers::createValidCard();
    card.setNickname("My Card 💳");

    ValidationResult result = manager->validateCard(card);
    QVERIFY(result.isValid);
}

// TC-UNIT-105: Nickname with spaces only
void TestPaymentCardManagerValidation::testValidateNickname_spacesOnly()
{
    PaymentCard card = TestHelpers::createValidCard();
    card.setNickname("   ");

    ValidationResult result = manager->validateCard(card);
    QVERIFY(!result.isValid);
    QVERIFY(result.errors.contains("nickname"));
}

// TC-UNIT-110: Valid issuer "Chase"
void TestPaymentCardManagerValidation::testValidateIssuer_valid()
{
    PaymentCard card = TestHelpers::createValidCard();
    card.setIssuer("Chase");

    ValidationResult result = manager->validateCard(card);
    QVERIFY(result.isValid);
}

// TC-UNIT-111: Valid issuer with 50 characters
void TestPaymentCardManagerValidation::testValidateIssuer_exactly50chars()
{
    PaymentCard card = TestHelpers::createValidCard();
    card.setIssuer(QString("A").repeated(50));

    ValidationResult result = manager->validateCard(card);
    QVERIFY(result.isValid);
}

// TC-UNIT-112: Empty issuer
void TestPaymentCardManagerValidation::testValidateIssuer_empty()
{
    PaymentCard card = TestHelpers::createValidCard();
    card.setIssuer("");

    ValidationResult result = manager->validateCard(card);
    QVERIFY(!result.isValid);
    QVERIFY(result.errors.contains("issuer"));
}

// TC-UNIT-113: Issuer exceeds 50 characters
void TestPaymentCardManagerValidation::testValidateIssuer_exceeds50chars()
{
    PaymentCard card = TestHelpers::createValidCard();
    card.setIssuer(QString("A").repeated(51));

    ValidationResult result = manager->validateCard(card);
    QVERIFY(!result.isValid);
    QVERIFY(result.errors.contains("issuer"));
}

// TC-UNIT-114: Issuer with spaces only
void TestPaymentCardManagerValidation::testValidateIssuer_spacesOnly()
{
    PaymentCard card = TestHelpers::createValidCard();
    card.setIssuer("   ");

    ValidationResult result = manager->validateCard(card);
    QVERIFY(!result.isValid);
}

// TC-UNIT-120: Valid last 4 digits "1234"
void TestPaymentCardManagerValidation::testValidateLast4_valid1234()
{
    PaymentCard card = TestHelpers::createValidCard();
    card.setLast4Digits("1234");

    ValidationResult result = manager->validateCard(card);
    QVERIFY(result.isValid);
}

// TC-UNIT-121: Valid last 4 digits "0000"
void TestPaymentCardManagerValidation::testValidateLast4_valid0000()
{
    PaymentCard card = TestHelpers::createValidCard();
    card.setLast4Digits("0000");

    ValidationResult result = manager->validateCard(card);
    QVERIFY(result.isValid);
}

// TC-UNIT-122: Valid last 4 digits "9999"
void TestPaymentCardManagerValidation::testValidateLast4_valid9999()
{
    PaymentCard card = TestHelpers::createValidCard();
    card.setLast4Digits("9999");

    ValidationResult result = manager->validateCard(card);
    QVERIFY(result.isValid);
}

// TC-UNIT-123: Empty last 4 digits
void TestPaymentCardManagerValidation::testValidateLast4_empty()
{
    PaymentCard card = TestHelpers::createValidCard();
    card.setLast4Digits("");

    ValidationResult result = manager->validateCard(card);
    QVERIFY(!result.isValid);
    QVERIFY(result.errors.contains("last4"));
}

// TC-UNIT-124: Last 4 digits with 3 characters
void TestPaymentCardManagerValidation::testValidateLast4_threeDigits()
{
    PaymentCard card = TestHelpers::createValidCard();
    card.setLast4Digits("123");

    ValidationResult result = manager->validateCard(card);
    QVERIFY(!result.isValid);
    QVERIFY(result.errors.contains("last4"));
    QVERIFY(result.errors["last4"].contains("4 digits"));
}

// TC-UNIT-125: Last 4 digits with 5 characters
void TestPaymentCardManagerValidation::testValidateLast4_fiveDigits()
{
    PaymentCard card = TestHelpers::createValidCard();
    card.setLast4Digits("12345");

    ValidationResult result = manager->validateCard(card);
    QVERIFY(!result.isValid);
    QVERIFY(result.errors.contains("last4"));
}

// TC-UNIT-126: Last 4 digits with letters
void TestPaymentCardManagerValidation::testValidateLast4_withLetters()
{
    PaymentCard card = TestHelpers::createValidCard();
    card.setLast4Digits("12ab");

    ValidationResult result = manager->validateCard(card);
    QVERIFY(!result.isValid);
    QVERIFY(result.errors.contains("last4"));
}

// TC-UNIT-127: Last 4 digits with special chars
void TestPaymentCardManagerValidation::testValidateLast4_withSpecialChars()
{
    PaymentCard card = TestHelpers::createValidCard();
    card.setLast4Digits("12@#");

    ValidationResult result = manager->validateCard(card);
    QVERIFY(!result.isValid);
}

// TC-UNIT-128: Last 4 digits with spaces
void TestPaymentCardManagerValidation::testValidateLast4_withSpaces()
{
    PaymentCard card = TestHelpers::createValidCard();
    card.setLast4Digits("12 34");

    ValidationResult result = manager->validateCard(card);
    QVERIFY(!result.isValid);
}

// TC-UNIT-130: Valid future date
void TestPaymentCardManagerValidation::testValidateExpiry_futureDate()
{
    PaymentCard card = TestHelpers::createValidCard();
    card.setExpiryDate(QDate(2028, 12, 1));

    ValidationResult result = manager->validateCard(card);
    QVERIFY(result.isValid);
}

// TC-UNIT-131: Valid date next month
void TestPaymentCardManagerValidation::testValidateExpiry_nextMonth()
{
    PaymentCard card = TestHelpers::createValidCard();
    card.setExpiryDate(QDate::currentDate().addMonths(1));

    ValidationResult result = manager->validateCard(card);
    QVERIFY(result.isValid);
}

// TC-UNIT-132: Current month
void TestPaymentCardManagerValidation::testValidateExpiry_currentMonth()
{
    PaymentCard card = TestHelpers::createValidCard();
    QDate current = QDate::currentDate();
    card.setExpiryDate(QDate(current.year(), current.month(), 1));

    ValidationResult result = manager->validateCard(card);
    QVERIFY(result.isValid);
}

// TC-UNIT-133: Past date
void TestPaymentCardManagerValidation::testValidateExpiry_pastDate()
{
    PaymentCard card = TestHelpers::createValidCard();
    card.setExpiryDate(QDate(2025, 1, 1));

    ValidationResult result = manager->validateCard(card);
    QVERIFY(!result.isValid);
    QVERIFY(result.errors.contains("expiry"));
    QVERIFY(result.errors["expiry"].contains("future"));
}

// TC-UNIT-134: Invalid month 0
void TestPaymentCardManagerValidation::testValidateExpiry_invalidMonth0()
{
    PaymentCard card = TestHelpers::createValidCard();
    card.setExpiryDate(QDate(2028, 0, 1));

    ValidationResult result = manager->validateCard(card);
    QVERIFY(!result.isValid);
    QVERIFY(result.errors.contains("expiry"));
}

// TC-UNIT-135: Invalid month 13
void TestPaymentCardManagerValidation::testValidateExpiry_invalidMonth13()
{
    PaymentCard card = TestHelpers::createValidCard();
    card.setExpiryDate(QDate(2028, 13, 1));

    ValidationResult result = manager->validateCard(card);
    QVERIFY(!result.isValid);
}

// TC-UNIT-136: Null date
void TestPaymentCardManagerValidation::testValidateExpiry_nullDate()
{
    PaymentCard card = TestHelpers::createValidCard();
    card.setExpiryDate(QDate());

    ValidationResult result = manager->validateCard(card);
    QVERIFY(!result.isValid);
    QVERIFY(result.errors.contains("expiry"));
}

// TC-UNIT-140: No CVC provided
void TestPaymentCardManagerValidation::testValidateCVC_empty()
{
    PaymentCard card = TestHelpers::createValidCard();

    ValidationResult result = manager->validateCard(card, "");
    QVERIFY(result.isValid); // CVC is optional
}

// TC-UNIT-141: Valid 3-digit CVC
void TestPaymentCardManagerValidation::testValidateCVC_valid3digit()
{
    PaymentCard card = TestHelpers::createValidCard();

    ValidationResult result = manager->validateCard(card, "123");
    QVERIFY(result.isValid);
}

// TC-UNIT-142: Valid 4-digit CVC
void TestPaymentCardManagerValidation::testValidateCVC_valid4digit()
{
    PaymentCard card = TestHelpers::createValidCard();

    ValidationResult result = manager->validateCard(card, "1234");
    QVERIFY(result.isValid);
}

// TC-UNIT-143: Invalid 2-digit CVC
void TestPaymentCardManagerValidation::testValidateCVC_invalid2digit()
{
    PaymentCard card = TestHelpers::createValidCard();

    ValidationResult result = manager->validateCard(card, "12");
    QVERIFY2(!result.isValid, "Validation should fail for 2-digit CVC");
    QVERIFY2(result.errors.contains("cvc"), "Should have CVC error");

    QString actualMessage = result.errors.value("cvc", "");
    QVERIFY2(actualMessage.contains("3") && actualMessage.contains("4"),
             qPrintable(QString("Expected '3-4' in message, got: %1").arg(actualMessage)));
}

// TC-UNIT-144: Invalid 5-digit CVC
void TestPaymentCardManagerValidation::testValidateCVC_invalid5digit()
{
    PaymentCard card = TestHelpers::createValidCard();

    ValidationResult result = manager->validateCard(card, "12345");
    QVERIFY(!result.isValid);
    QVERIFY(result.errors.contains("cvc"));
}

// TC-UNIT-145: CVC with letters
void TestPaymentCardManagerValidation::testValidateCVC_withLetters()
{
    PaymentCard card = TestHelpers::createValidCard();

    ValidationResult result = manager->validateCard(card, "12a");
    QVERIFY(!result.isValid);
}

// TC-UNIT-146: CVC with special chars
void TestPaymentCardManagerValidation::testValidateCVC_withSpecialChars()
{
    PaymentCard card = TestHelpers::createValidCard();

    ValidationResult result = manager->validateCard(card, "12@");
    QVERIFY(!result.isValid);
}

// TC-UNIT-150: All valid fields without CVC
void TestPaymentCardManagerValidation::testValidateCard_allValidWithoutCVC()
{
    PaymentCard card;
    card.setNickname("My Visa");
    card.setIssuer("Chase");
    card.setLast4Digits("1234");
    card.setExpiryDate(QDate(2028, 12, 1));

    ValidationResult result = manager->validateCard(card, "");
    QVERIFY(result.isValid);
    QCOMPARE(result.errors.size(), 0);
}

// TC-UNIT-151: All valid fields with CVC
void TestPaymentCardManagerValidation::testValidateCard_allValidWithCVC()
{
    PaymentCard card;
    card.setNickname("My Visa");
    card.setIssuer("Chase");
    card.setLast4Digits("1234");
    card.setExpiryDate(QDate(2028, 12, 1));

    ValidationResult result = manager->validateCard(card, "123");
    QVERIFY(result.isValid);
}

// TC-UNIT-152: Multiple invalid fields
void TestPaymentCardManagerValidation::testValidateCard_multipleInvalidFields()
{
    PaymentCard card;
    card.setNickname("");
    card.setIssuer("");
    card.setLast4Digits("");
    card.setExpiryDate(QDate());

    ValidationResult result = manager->validateCard(card);
    QVERIFY(!result.isValid);
    QVERIFY(result.errors.size() > 1);
}

// TC-UNIT-153: One invalid field
void TestPaymentCardManagerValidation::testValidateCard_oneInvalidField()
{
    PaymentCard card = TestHelpers::createValidCard();
    card.setLast4Digits("123"); // Invalid: only 3 digits

    ValidationResult result = manager->validateCard(card);
    QVERIFY(!result.isValid);
    QVERIFY(result.errors.contains("last4"));
}

QTEST_MAIN(TestPaymentCardManagerValidation)
#include "tst_payment_card_manager_validation.moc"
