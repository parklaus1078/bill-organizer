#pragma once

#include <QString>
#include <QByteArray>

/**
 * @brief Handles encryption and decryption of sensitive card data
 *
 * CardEncryption provides AES-256-CBC encryption for CVC storage.
 * The encryption key is stored in platform-specific secure storage:
 * - macOS: Keychain
 * - Windows: Credential Manager
 * - Linux: In-memory for this implementation (consider libsecret for production)
 */
class CardEncryption
{
public:
    static CardEncryption *instance();

    /**
     * @brief Encrypts plain text using AES-256-CBC
     * @param plainText The text to encrypt
     * @return Encrypted data with IV prepended
     */
    QByteArray encrypt(const QString &plainText);

    /**
     * @brief Decrypts cipher text using AES-256-CBC
     * @param cipherText The encrypted data (with IV prepended)
     * @return Decrypted plain text
     */
    QString decrypt(const QByteArray &cipherText);

    /**
     * @brief Initializes or retrieves the encryption key
     * @return true if key is initialized successfully
     */
    bool initializeKey();

    /**
     * @brief Checks if a valid encryption key exists
     * @return true if key is available
     */
    bool hasValidKey() const;

private:
    CardEncryption();
    ~CardEncryption();

    // Singleton instance
    static CardEncryption *s_instance;

    QByteArray m_encryptionKey;
    bool m_keyInitialized;

    // Helper methods
    QByteArray generateKey();
    bool loadKeyFromSecureStorage();
    bool saveKeyToSecureStorage(const QByteArray &key);
};
