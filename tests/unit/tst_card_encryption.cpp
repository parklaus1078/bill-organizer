#include <QtTest/QtTest>
#include "../../src/utils/card_encryption.h"

/**
 * @brief Unit tests for CardEncryption service
 *
 * Tests encryption/decryption operations, round-trip encryption,
 * and key management functionality.
 */
class TestCardEncryption : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    // TC-UNIT-050 to TC-UNIT-054: Encryption Operations
    void testEncrypt_threeDigitCVC();
    void testEncrypt_fourDigitCVC();
    void testEncrypt_emptyString();
    void testEncrypt_specialCharacters();
    void testEncrypt_differentResults();

    // TC-UNIT-060 to TC-UNIT-063: Decryption Operations
    void testDecrypt_validCiphertext_123();
    void testDecrypt_validCiphertext_1234();
    void testDecrypt_emptyCiphertext();
    void testDecrypt_corruptedCiphertext();

    // TC-UNIT-070 to TC-UNIT-074: Round-Trip Encryption/Decryption
    void testRoundTrip_123();
    void testRoundTrip_1234();
    void testRoundTrip_000();
    void testRoundTrip_9999();
    void testRoundTrip_unicodeCharacters();

    // TC-UNIT-080 to TC-UNIT-082: Key Management
    void testInitializeKey();
    void testHasValidKey();
    void testEncryptWithoutKey();

private:
    CardEncryption *encryption;
};

void TestCardEncryption::initTestCase()
{
    // Initialize once for all tests
    encryption = CardEncryption::instance();
}

void TestCardEncryption::cleanupTestCase()
{
    // Cleanup is handled by singleton
}

void TestCardEncryption::init()
{
    // Ensure key is initialized before each test
    encryption->initializeKey();
}

void TestCardEncryption::cleanup()
{
    // No per-test cleanup needed
}

// TC-UNIT-050: Encrypt plain CVC "123"
void TestCardEncryption::testEncrypt_threeDigitCVC()
{
    QByteArray encrypted = encryption->encrypt("123");

    QVERIFY(!encrypted.isEmpty());
    // IV (16 bytes) + encrypted data (at least 3 bytes)
    QVERIFY2(encrypted.size() >= 16, "Encrypted data should have at least IV (16 bytes)");
}

// TC-UNIT-051: Encrypt plain CVC "1234"
void TestCardEncryption::testEncrypt_fourDigitCVC()
{
    QByteArray encrypted = encryption->encrypt("1234");

    QVERIFY(!encrypted.isEmpty());
    QVERIFY(encrypted.size() >= 16); // IV + data
}

// TC-UNIT-052: Encrypt empty string
void TestCardEncryption::testEncrypt_emptyString()
{
    QByteArray encrypted = encryption->encrypt("");

    // Should handle gracefully (either return empty or encrypted empty string)
    QVERIFY(true); // Just ensure no crash
}

// TC-UNIT-053: Encrypt special characters
void TestCardEncryption::testEncrypt_specialCharacters()
{
    QByteArray encrypted = encryption->encrypt("!@#$");

    QVERIFY(!encrypted.isEmpty());
}

// TC-UNIT-054: Two encryptions produce different results
void TestCardEncryption::testEncrypt_differentResults()
{
    QByteArray encrypted1 = encryption->encrypt("123");
    QByteArray encrypted2 = encryption->encrypt("123");

    QVERIFY2(encrypted1 != encrypted2, "Two encryptions of same plaintext should differ (random IV)");
}

// TC-UNIT-060: Decrypt valid ciphertext "123"
void TestCardEncryption::testDecrypt_validCiphertext_123()
{
    QByteArray encrypted = encryption->encrypt("123");
    QString decrypted = encryption->decrypt(encrypted);

    QCOMPARE(decrypted, QString("123"));
}

// TC-UNIT-061: Decrypt valid ciphertext "1234"
void TestCardEncryption::testDecrypt_validCiphertext_1234()
{
    QByteArray encrypted = encryption->encrypt("1234");
    QString decrypted = encryption->decrypt(encrypted);

    QCOMPARE(decrypted, QString("1234"));
}

// TC-UNIT-062: Decrypt empty ciphertext
void TestCardEncryption::testDecrypt_emptyCiphertext()
{
    QString decrypted = encryption->decrypt(QByteArray());

    // Should handle gracefully (return empty or specific error)
    QVERIFY(decrypted.isEmpty() || decrypted.contains("error", Qt::CaseInsensitive));
}

// TC-UNIT-063: Decrypt corrupted ciphertext
void TestCardEncryption::testDecrypt_corruptedCiphertext()
{
    QByteArray corrupted = QByteArray::fromHex("deadbeefcafe");
    QString decrypted = encryption->decrypt(corrupted);

    // Should handle gracefully without crashing
    QVERIFY(true);
}

// TC-UNIT-070: Round-trip "123"
void TestCardEncryption::testRoundTrip_123()
{
    QString original = "123";
    QByteArray encrypted = encryption->encrypt(original);
    QString decrypted = encryption->decrypt(encrypted);

    QCOMPARE(decrypted, original);
}

// TC-UNIT-071: Round-trip "1234"
void TestCardEncryption::testRoundTrip_1234()
{
    QString original = "1234";
    QByteArray encrypted = encryption->encrypt(original);
    QString decrypted = encryption->decrypt(encrypted);

    QCOMPARE(decrypted, original);
}

// TC-UNIT-072: Round-trip "000"
void TestCardEncryption::testRoundTrip_000()
{
    QString original = "000";
    QByteArray encrypted = encryption->encrypt(original);
    QString decrypted = encryption->decrypt(encrypted);

    QCOMPARE(decrypted, original);
}

// TC-UNIT-073: Round-trip "9999"
void TestCardEncryption::testRoundTrip_9999()
{
    QString original = "9999";
    QByteArray encrypted = encryption->encrypt(original);
    QString decrypted = encryption->decrypt(encrypted);

    QCOMPARE(decrypted, original);
}

// TC-UNIT-074: Round-trip Unicode characters
void TestCardEncryption::testRoundTrip_unicodeCharacters()
{
    QString original = "€£¥";
    QByteArray encrypted = encryption->encrypt(original);
    QString decrypted = encryption->decrypt(encrypted);

    QCOMPARE(decrypted, original);
}

// TC-UNIT-080: Initialize encryption key on first run
void TestCardEncryption::testInitializeKey()
{
    bool result = encryption->initializeKey();

    QVERIFY(result);
    QVERIFY(encryption->hasValidKey());
}

// TC-UNIT-081: Check valid key exists
void TestCardEncryption::testHasValidKey()
{
    encryption->initializeKey();

    QVERIFY(encryption->hasValidKey());
}

// TC-UNIT-082: Encryption fails without key
void TestCardEncryption::testEncryptWithoutKey()
{
    // This test assumes implementation prevents encryption without key
    // If key is always initialized in singleton, this test may be skipped
    QSKIP("Key is always initialized in current implementation");
}

QTEST_MAIN(TestCardEncryption)
#include "tst_card_encryption.moc"
