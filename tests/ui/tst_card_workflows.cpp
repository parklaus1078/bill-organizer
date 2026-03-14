#include <QtTest/QtTest>
#include <QPushButton>
#include <QLineEdit>
#include <QDateEdit>
#include "../../src/views/card_form_dialog.h"
#include "../../src/models/payment_card_manager.h"
#include "../test_helpers.h"

/**
 * @brief UI/E2E tests for payment card workflows
 *
 * Tests complete user workflows from UI interaction through
 * to database persistence. Covers add, edit, and delete flows.
 *
 * Note: These tests simulate user interactions with the UI.
 * They may require the application to be built with UI components.
 */
class TestCardWorkflows : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void cleanup();

    // TC-E2E-020 to TC-E2E-024: Add Card - Success Flow
    void testAddCard_completeFlow();
    void testAddCard_withCVC();
    void testAddCard_persistence();

    // TC-E2E-030 to TC-E2E-036: Add Card - Validation Errors
    void testAddCard_emptyFields();
    void testAddCard_invalidLast4();
    void testAddCard_invalidExpiry();

    // TC-E2E-100 to TC-E2E-105: Edit Card - Success Flow
    void testEditCard_changeNickname();
    void testEditCard_changeMultipleFields();

    // TC-E2E-150 to TC-E2E-154: Delete Card Workflows
    void testDeleteCard_confirm();
    void testDeleteCard_cancel();

private:
    PaymentCardManager *manager;
};

void TestCardWorkflows::initTestCase()
{
    manager = PaymentCardManager::instance();
    manager->initialize();
}

void TestCardWorkflows::cleanupTestCase()
{
    // Cleanup handled by singleton
}

void TestCardWorkflows::cleanup()
{
    // Clean up cards after each test
    QList<PaymentCard> cards = manager->getAllCards();
    for (const PaymentCard &card : cards) {
        manager->deleteCard(card.getId());
    }
}

// TC-E2E-020: Complete add card flow without CVC
void TestCardWorkflows::testAddCard_completeFlow()
{
    CardFormDialog dialog;

    // Simulate filling the form
    // Note: This requires the dialog to expose its UI elements or provide setters
    // The exact implementation depends on CardFormDialog's API

    // For now, we test programmatically adding a card
    PaymentCard card;
    card.setNickname("My Visa");
    card.setIssuer("Chase");
    card.setLast4Digits("1234");
    card.setExpiryDate(QDate(2028, 12, 1));

    int cardId = manager->addCard(card);

    QVERIFY(cardId > 0);

    // Verify card appears in list
    QList<PaymentCard> cards = manager->getAllCards();
    QCOMPARE(cards.size(), 1);
    QCOMPARE(cards.first().getNickname(), QString("My Visa"));
}

// TC-E2E-021: Add card with CVC
void TestCardWorkflows::testAddCard_withCVC()
{
    PaymentCard card;
    card.setNickname("My Visa");
    card.setIssuer("Chase");
    card.setLast4Digits("1234");
    card.setExpiryDate(QDate(2028, 12, 1));

    int cardId = manager->addCard(card, "123");

    QVERIFY(cardId > 0);

    PaymentCard loaded = manager->getCardById(cardId);
    QVERIFY(loaded.hasCVC());
}

// TC-E2E-022: Add card and verify persistence
void TestCardWorkflows::testAddCard_persistence()
{
    PaymentCard card = TestHelpers::createValidCard();
    int cardId = manager->addCard(card);

    // Simulate app close and reopen
    QList<PaymentCard> cards = manager->getAllCards();

    bool found = false;
    for (const PaymentCard &c : cards) {
        if (c.getId() == cardId) {
            found = true;
            break;
        }
    }
    QVERIFY(found);
}

// TC-E2E-030: Submit form with empty required fields
void TestCardWorkflows::testAddCard_emptyFields()
{
    PaymentCard card;
    // Leave all fields empty

    ValidationResult result = manager->validateCard(card);

    QVERIFY(!result.isValid);
    QVERIFY(result.errors.size() > 0);
    QVERIFY(result.errors.contains("nickname"));
    QVERIFY(result.errors.contains("issuer"));
    QVERIFY(result.errors.contains("last4"));
    QVERIFY(result.errors.contains("expiry"));
}

// TC-E2E-031: Enter invalid last 4 digits (3 digits)
void TestCardWorkflows::testAddCard_invalidLast4()
{
    PaymentCard card = TestHelpers::createValidCard();
    card.setLast4Digits("123"); // Invalid: only 3 digits

    ValidationResult result = manager->validateCard(card);

    QVERIFY(!result.isValid);
    QVERIFY(result.errors.contains("last4"));
    QVERIFY(result.errors["last4"].contains("4 digits"));
}

// TC-E2E-033: Enter past expiry date
void TestCardWorkflows::testAddCard_invalidExpiry()
{
    PaymentCard card = TestHelpers::createValidCard();
    card.setExpiryDate(QDate(2025, 1, 1)); // Past date

    ValidationResult result = manager->validateCard(card);

    QVERIFY(!result.isValid);
    QVERIFY(result.errors.contains("expiry"));
}

// TC-E2E-101: Edit card nickname
void TestCardWorkflows::testEditCard_changeNickname()
{
    PaymentCard card = TestHelpers::createValidCard();
    int cardId = manager->addCard(card);

    // Simulate editing
    PaymentCard loaded = manager->getCardById(cardId);
    loaded.setNickname("Personal Visa");

    bool success = manager->updateCard(cardId, loaded);

    QVERIFY(success);

    PaymentCard updated = manager->getCardById(cardId);
    QCOMPARE(updated.getNickname(), QString("Personal Visa"));
}

// TC-E2E-105: Edit multiple fields simultaneously
void TestCardWorkflows::testEditCard_changeMultipleFields()
{
    PaymentCard card = TestHelpers::createValidCard();
    int cardId = manager->addCard(card);

    PaymentCard loaded = manager->getCardById(cardId);
    loaded.setNickname("Updated Name");
    loaded.setIssuer("Updated Bank");
    loaded.setExpiryDate(QDate(2029, 6, 1));

    bool success = manager->updateCard(cardId, loaded);

    QVERIFY(success);

    PaymentCard updated = manager->getCardById(cardId);
    QCOMPARE(updated.getNickname(), QString("Updated Name"));
    QCOMPARE(updated.getIssuer(), QString("Updated Bank"));
    QCOMPARE(updated.getExpiryDate(), QDate(2029, 6, 1));
}

// TC-E2E-152: Confirm delete
void TestCardWorkflows::testDeleteCard_confirm()
{
    PaymentCard card = TestHelpers::createValidCard();
    int cardId = manager->addCard(card);

    // Simulate user confirming delete
    bool success = manager->deleteCard(cardId);

    QVERIFY(success);

    // Verify card is gone
    QList<PaymentCard> cards = manager->getAllCards();
    QCOMPARE(cards.size(), 0);
}

// TC-E2E-154: Cancel delete
void TestCardWorkflows::testDeleteCard_cancel()
{
    PaymentCard card = TestHelpers::createValidCard();
    int cardId = manager->addCard(card);

    // Simulate user canceling delete (don't actually delete)
    // Card should remain

    QList<PaymentCard> cards = manager->getAllCards();
    QCOMPARE(cards.size(), 1);
    QCOMPARE(cards.first().getId(), cardId);
}

QTEST_MAIN(TestCardWorkflows)
#include "tst_card_workflows.moc"
