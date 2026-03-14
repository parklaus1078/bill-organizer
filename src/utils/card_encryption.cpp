#include "card_encryption.h"
#include <QSettings>
#include <QCryptographicHash>
#include <QRandomGenerator>
#include <QtDebug>

CardEncryption *CardEncryption::s_instance = nullptr;

CardEncryption::CardEncryption()
    : m_keyInitialized(false)
{
}

CardEncryption::~CardEncryption()
{
    // Clear sensitive data from memory
    m_encryptionKey.fill(0);
}

CardEncryption *CardEncryption::instance()
{
    if (!s_instance) {
        s_instance = new CardEncryption();
        s_instance->initializeKey();
    }
    return s_instance;
}

bool CardEncryption::initializeKey()
{
    if (m_keyInitialized) {
        return true;
    }

    // Try to load existing key
    if (loadKeyFromSecureStorage()) {
        m_keyInitialized = true;
        return true;
    }

    // Generate new key if none exists
    m_encryptionKey = generateKey();
    if (saveKeyToSecureStorage(m_encryptionKey)) {
        m_keyInitialized = true;
        return true;
    }

    qWarning() << "Failed to initialize encryption key";
    return false;
}

bool CardEncryption::hasValidKey() const
{
    return m_keyInitialized && !m_encryptionKey.isEmpty();
}

QByteArray CardEncryption::generateKey()
{
    // Generate a 32-byte (256-bit) random key
    QByteArray key;
    key.resize(32);

    for (int i = 0; i < 32; ++i) {
        key[i] = static_cast<char>(QRandomGenerator::global()->bounded(256));
    }

    return key;
}

bool CardEncryption::loadKeyFromSecureStorage()
{
    // For this implementation, we'll store in QSettings
    // In production, use platform-specific secure storage:
    // - macOS: Security framework (Keychain)
    // - Windows: DPAPI
    // - Linux: libsecret or KWallet

    QSettings settings("BillOrganizer", "BillOrganizer");
    QByteArray storedKey = settings.value("encryption/key").toByteArray();

    if (!storedKey.isEmpty()) {
        m_encryptionKey = storedKey;
        return true;
    }

    return false;
}

bool CardEncryption::saveKeyToSecureStorage(const QByteArray &key)
{
    // Store in QSettings
    // NOTE: This is NOT secure for production use
    // TODO: Implement platform-specific secure storage
    QSettings settings("BillOrganizer", "BillOrganizer");
    settings.setValue("encryption/key", key);
    settings.sync();

    return settings.status() == QSettings::NoError;
}

QByteArray CardEncryption::encrypt(const QString &plainText)
{
    if (!hasValidKey()) {
        qWarning() << "Cannot encrypt: no valid key";
        return QByteArray();
    }

    if (plainText.isEmpty()) {
        return QByteArray();
    }

    // Simple XOR-based encryption for this implementation
    // In production, use QCA (Qt Cryptographic Architecture) or OpenSSL for AES-256
    QByteArray data = plainText.toUtf8();
    QByteArray result;

    // Generate random IV (16 bytes for AES)
    QByteArray iv;
    iv.resize(16);
    for (int i = 0; i < 16; ++i) {
        iv[i] = static_cast<char>(QRandomGenerator::global()->bounded(256));
    }

    // Prepend IV to result
    result.append(iv);

    // Simple XOR encryption (replace with proper AES in production)
    for (int i = 0; i < data.size(); ++i) {
        char encrypted = data[i] ^ m_encryptionKey[i % m_encryptionKey.size()] ^ iv[i % iv.size()];
        result.append(encrypted);
    }

    return result;
}

QString CardEncryption::decrypt(const QByteArray &cipherText)
{
    if (!hasValidKey()) {
        qWarning() << "Cannot decrypt: no valid key";
        return QString();
    }

    if (cipherText.isEmpty() || cipherText.size() < 16) {
        return QString();
    }

    // Extract IV (first 16 bytes)
    QByteArray iv = cipherText.left(16);
    QByteArray encryptedData = cipherText.mid(16);

    // Decrypt using XOR (reverse of encryption)
    QByteArray decrypted;
    for (int i = 0; i < encryptedData.size(); ++i) {
        char decryptedChar = encryptedData[i] ^ m_encryptionKey[i % m_encryptionKey.size()] ^ iv[i % iv.size()];
        decrypted.append(decryptedChar);
    }

    return QString::fromUtf8(decrypted);
}
