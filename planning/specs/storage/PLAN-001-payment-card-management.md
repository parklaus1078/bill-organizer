# Payment Card Management - Database & Encryption Specification

## Overview
This document specifies the database schema, encryption implementation, and data access layer for payment card storage in the Bill Organizer desktop application.

---

## Database Technology

**DBMS**: SQLite 3
- **Rationale**: Embedded database, no server required, cross-platform, good Qt support
- **Qt Integration**: Use `QtSql` module (`QSqlDatabase`, `QSqlQuery`)
- **File Location**: `~/.local/share/BillOrganizer/data.db` (Linux/macOS) or `%APPDATA%\BillOrganizer\data.db` (Windows)

---

## Database Schema

### Table: `payment_cards`

```sql
CREATE TABLE payment_cards (
    id                  INTEGER PRIMARY KEY AUTOINCREMENT,
    nickname            TEXT NOT NULL,
    issuer              TEXT NOT NULL,
    last4_digits        TEXT NOT NULL,
    expiry_month        INTEGER NOT NULL CHECK(expiry_month BETWEEN 1 AND 12),
    expiry_year         INTEGER NOT NULL CHECK(expiry_year >= 2000),
    encrypted_cvc       BLOB NULL,
    created_at          TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP,
    updated_at          TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP
);

-- Index for duplicate detection
CREATE INDEX idx_last4_expiry ON payment_cards(last4_digits, expiry_month, expiry_year);

-- Index for sorting by creation date
CREATE INDEX idx_created_at ON payment_cards(created_at DESC);
```

**Column Details:**

| Column | Type | Nullable | Description |
|--------|------|----------|-------------|
| `id` | INTEGER | No | Auto-incrementing primary key |
| `nickname` | TEXT | No | User-defined card nickname (max 50 chars) |
| `issuer` | TEXT | No | Bank/card issuer name (max 50 chars) |
| `last4_digits` | TEXT | No | Last 4 digits of card number (4 chars, numeric) |
| `expiry_month` | INTEGER | No | Expiry month (1-12) |
| `expiry_year` | INTEGER | No | Expiry year (YYYY format, e.g., 2028) |
| `encrypted_cvc` | BLOB | Yes | AES-256 encrypted CVC (optional, can be NULL) |
| `created_at` | TEXT | No | ISO 8601 timestamp when card was created |
| `updated_at` | TEXT | No | ISO 8601 timestamp when card was last modified |

**Constraints:**
- `expiry_month`: Must be between 1 and 12
- `expiry_year`: Must be >= 2000 (prevents invalid years)
- No unique constraint on (last4_digits, expiry_month, expiry_year) to allow duplicates (user may have multiple cards with same visible details)

---

### Future Tables (for reference, not implemented in PLAN-001)

**Table: `subscriptions`** (PLAN-002)
```sql
CREATE TABLE subscriptions (
    id                  INTEGER PRIMARY KEY AUTOINCREMENT,
    name                TEXT NOT NULL,
    payment_card_id     INTEGER NULL,  -- Foreign key to payment_cards
    -- ... other fields
    FOREIGN KEY (payment_card_id) REFERENCES payment_cards(id) ON DELETE SET NULL
);
```

**Note**: When a card is deleted, subscriptions linked to it will have `payment_card_id` set to NULL (not cascade delete).

---

## Encryption Specification

### Encryption Algorithm
- **Algorithm**: AES-256-CBC (Cipher Block Chaining)
- **Key Size**: 256 bits (32 bytes)
- **Block Size**: 128 bits (16 bytes)
- **Padding**: PKCS7

### Implementation Details

#### Qt Encryption Libraries
**Option 1: Qt Cryptographic Architecture (QCA)**
- Library: `qca-qt6` (Qt Cryptographic Architecture)
- Install: `apt-get install libqca-qt6-2-dev` (Linux) or via vcpkg/homebrew
- Usage:
  ```cpp
  #include <QtCrypto>
  QCA::Cipher cipher("aes256", QCA::Cipher::CBC, QCA::Cipher::DefaultPadding);
  ```

**Option 2: OpenSSL via Qt**
- Library: OpenSSL (bundled with most Qt installations)
- Qt Wrapper: Use `QSslCipher` or direct OpenSSL API
- Usage:
  ```cpp
  #include <openssl/evp.h>
  // Use EVP_EncryptInit_ex, EVP_EncryptUpdate, EVP_EncryptFinal_ex
  ```

**Recommended**: QCA for simpler API, OpenSSL for more control.

---

### Encryption Process

#### 1. Key Generation and Storage

**Key Generation** (first run only):
```cpp
QByteArray generateEncryptionKey() {
    // Generate 256-bit (32 byte) random key
    QCA::Random random;
    return random.randomArray(32);
}
```

**Key Storage**:
- **Library**: `qtkeychain` (QtKeychain)
  - GitHub: https://github.com/frankosterfeld/qtkeychain
  - Platform support: macOS Keychain, Windows Credential Manager, Linux libsecret
- **Code**:
  ```cpp
  #include <qt6keychain/keychain.h>

  void storeKey(const QByteArray& key) {
      QKeychain::WritePasswordJob job("BillOrganizer");
      job.setKey("encryption_key");
      job.setBinaryData(key);
      job.start();
      job.waitForFinished();  // Or use async signals
  }

  QByteArray loadKey() {
      QKeychain::ReadPasswordJob job("BillOrganizer");
      job.setKey("encryption_key");
      job.start();
      job.waitForFinished();
      return job.binaryData();
  }
  ```

**Fallback** (if QtKeychain not available):
- Store key in user config directory with restricted file permissions (chmod 600)
- **Warning**: Less secure, vulnerable to file system access

---

#### 2. CVC Encryption

**Input**: Plain-text CVC (3-4 digits, e.g., "123")

**Process**:
1. Generate random IV (Initialization Vector): 16 bytes
2. Encrypt CVC using AES-256-CBC with key + IV
3. Prepend IV to ciphertext: `[IV (16 bytes)][Ciphertext (variable)]`
4. Store resulting BLOB in database

**Code**:
```cpp
QByteArray encryptCVC(const QString& plainCVC, const QByteArray& key) {
    // Generate random IV
    QCA::Random random;
    QByteArray iv = random.randomArray(16);

    // Initialize cipher
    QCA::Cipher cipher("aes256", QCA::Cipher::CBC, QCA::Cipher::DefaultPadding);
    QCA::InitializationVector qcaIv(iv);
    QCA::SymmetricKey qcaKey(key);

    cipher.setup(QCA::Encode, qcaKey, qcaIv);

    // Encrypt
    QCA::SecureArray plainData(plainCVC.toUtf8());
    QCA::SecureArray cipherData = cipher.update(plainData);
    cipherData += cipher.final();

    // Prepend IV to ciphertext
    QByteArray result = iv;
    result.append(cipherData.toByteArray());

    return result;
}
```

---

#### 3. CVC Decryption

**Input**: Encrypted BLOB from database

**Process**:
1. Extract IV from first 16 bytes
2. Extract ciphertext from remaining bytes
3. Decrypt using AES-256-CBC with key + IV
4. Return plain-text CVC

**Code**:
```cpp
QString decryptCVC(const QByteArray& encrypted, const QByteArray& key) {
    // Extract IV (first 16 bytes)
    QByteArray iv = encrypted.left(16);

    // Extract ciphertext (remaining bytes)
    QByteArray ciphertext = encrypted.mid(16);

    // Initialize cipher
    QCA::Cipher cipher("aes256", QCA::Cipher::CBC, QCA::Cipher::DefaultPadding);
    QCA::InitializationVector qcaIv(iv);
    QCA::SymmetricKey qcaKey(key);

    cipher.setup(QCA::Decode, qcaKey, qcaIv);

    // Decrypt
    QCA::SecureArray plainData = cipher.update(QCA::SecureArray(ciphertext));
    plainData += cipher.final();

    return QString::fromUtf8(plainData.toByteArray());
}
```

---

### Encryption Error Handling

**Scenarios**:
1. **Key Not Found**: First run or key deleted
   - Action: Generate new key, warn user that existing encrypted data cannot be decrypted
2. **Decryption Failure**: Corrupted data or wrong key
   - Action: Log error, show generic "Unable to decrypt CVC" message, allow user to re-enter CVC
3. **Encryption Failure**: Library error
   - Action: Log error, prevent saving card, show error message to user

---

## Data Access Layer (DAL)

### Class: `CardDatabase`

**Responsibilities**:
- Execute SQL queries
- Map database rows to `PaymentCard` objects
- Handle database transactions

**Interface**:
```cpp
class CardDatabase {
public:
    CardDatabase();
    ~CardDatabase();

    // Initialization
    bool initialize(const QString& dbPath);
    bool createTables();

    // CRUD operations
    int insertCard(const PaymentCard& card);
    bool updateCard(int id, const PaymentCard& card);
    bool deleteCard(int id);
    PaymentCard selectCardById(int id);
    QList<PaymentCard> selectAllCards();

    // Queries
    bool isDuplicate(const QString& last4, int month, int year, int excludeId = -1);
    int countSubscriptionsByCardId(int cardId);  // Future implementation

private:
    QSqlDatabase db;
    bool executeQuery(QSqlQuery& query);
    PaymentCard mapRowToCard(const QSqlQuery& query);
};
```

---

### SQL Queries

#### Insert Card
```sql
INSERT INTO payment_cards (nickname, issuer, last4_digits, expiry_month, expiry_year, encrypted_cvc)
VALUES (?, ?, ?, ?, ?, ?);
```

**Parameters**:
1. `nickname`: QString
2. `issuer`: QString
3. `last4_digits`: QString (4 chars)
4. `expiry_month`: int (1-12)
5. `expiry_year`: int (YYYY)
6. `encrypted_cvc`: QByteArray (BLOB, can be NULL)

**Return**: Last inserted row ID (`query.lastInsertId().toInt()`)

---

#### Update Card
```sql
UPDATE payment_cards
SET nickname = ?, issuer = ?, last4_digits = ?, expiry_month = ?, expiry_year = ?,
    encrypted_cvc = ?, updated_at = CURRENT_TIMESTAMP
WHERE id = ?;
```

**Parameters**:
1-6: Same as insert
7. `id`: int

**Return**: Number of rows affected (`query.numRowsAffected()`)

---

#### Delete Card
```sql
DELETE FROM payment_cards WHERE id = ?;
```

**Parameters**:
1. `id`: int

**Return**: Number of rows affected

---

#### Select Card by ID
```sql
SELECT id, nickname, issuer, last4_digits, expiry_month, expiry_year, encrypted_cvc, created_at, updated_at
FROM payment_cards
WHERE id = ?;
```

**Parameters**:
1. `id`: int

**Return**: Single `PaymentCard` object or null if not found

---

#### Select All Cards
```sql
SELECT id, nickname, issuer, last4_digits, expiry_month, expiry_year, encrypted_cvc, created_at, updated_at
FROM payment_cards
ORDER BY created_at DESC;
```

**Return**: `QList<PaymentCard>`

---

#### Check Duplicate
```sql
SELECT COUNT(*) FROM payment_cards
WHERE last4_digits = ? AND expiry_month = ? AND expiry_year = ? AND id != ?;
```

**Parameters**:
1. `last4_digits`: QString
2. `expiry_month`: int
3. `expiry_year`: int
4. `excludeId`: int (use -1 to check all, or specific ID to exclude during update)

**Return**: bool (true if count > 0)

---

#### Count Subscriptions by Card ID (Future)
```sql
SELECT COUNT(*) FROM subscriptions WHERE payment_card_id = ?;
```

**Parameters**:
1. `cardId`: int

**Return**: int (subscription count)

---

## Data Mapping

### Database Row → PaymentCard Object

```cpp
PaymentCard CardDatabase::mapRowToCard(const QSqlQuery& query) {
    PaymentCard card;
    card.setId(query.value("id").toInt());
    card.setNickname(query.value("nickname").toString());
    card.setIssuer(query.value("issuer").toString());
    card.setLast4Digits(query.value("last4_digits").toString());

    int month = query.value("expiry_month").toInt();
    int year = query.value("expiry_year").toInt();
    card.setExpiryDate(QDate(year, month, 1));  // Store as first day of expiry month

    if (!query.value("encrypted_cvc").isNull()) {
        card.setEncryptedCVC(query.value("encrypted_cvc").toByteArray());
    }

    card.setCreatedAt(QDateTime::fromString(query.value("created_at").toString(), Qt::ISODate));
    card.setUpdatedAt(QDateTime::fromString(query.value("updated_at").toString(), Qt::ISODate));

    return card;
}
```

---

## Transaction Handling

### Use Cases for Transactions

**Single Card Operations**: No transaction needed (single query, atomic by default)

**Batch Operations** (future enhancement):
```cpp
bool CardDatabase::importCards(const QList<PaymentCard>& cards) {
    db.transaction();

    for (const auto& card : cards) {
        if (insertCard(card) == -1) {
            db.rollback();
            return false;
        }
    }

    return db.commit();
}
```

---

## Database Migration Strategy

### Initial Setup (v1.0)
- Run `CREATE TABLE` on first launch
- Check if table exists: `SELECT name FROM sqlite_master WHERE type='table' AND name='payment_cards';`

### Future Migrations (v1.1+)
- Use schema version table:
  ```sql
  CREATE TABLE schema_version (version INTEGER);
  INSERT INTO schema_version VALUES (1);
  ```
- Migration scripts:
  ```cpp
  void CardDatabase::migrateToVersion2() {
      // Example: Add new column
      db.exec("ALTER TABLE payment_cards ADD COLUMN card_color TEXT DEFAULT '#000000';");
      db.exec("UPDATE schema_version SET version = 2;");
  }
  ```

---

## Performance Optimization

### Prepared Statements
- Use `QSqlQuery::prepare()` and `QSqlQuery::bindValue()` for all queries
- Prevents SQL injection
- Improves performance for repeated queries

### Indexing
- Index on `(last4_digits, expiry_month, expiry_year)` for duplicate detection: ~O(log n)
- Index on `created_at` for sorted card list: ~O(log n)

### Query Optimization
- Avoid `SELECT *`: Explicitly list columns
- Use `LIMIT` for pagination (future enhancement)

---

## Backup and Restore

### Backup Strategy
- **Manual Backup**: Copy `data.db` file to user-selected location
- **Automated Backup**: Daily backup to `~/.local/share/BillOrganizer/backups/` (keep last 7 days)

### Restore Process
1. Close database connection
2. Replace `data.db` with backup file
3. Reopen database
4. Verify integrity: `PRAGMA integrity_check;`

### Data Export (Future Enhancement)
- Export to JSON (encrypted CVC as base64):
  ```json
  {
    "cards": [
      {
        "nickname": "My Visa",
        "issuer": "Chase",
        "last4": "1234",
        "expiry": "12/28",
        "encrypted_cvc": "base64_encoded_blob"
      }
    ]
  }
  ```

---

## Security Considerations

### SQL Injection Prevention
- **Always use prepared statements** with parameter binding
- Never concatenate user input into SQL strings

### Data at Rest
- **Encrypted**: CVC (AES-256)
- **Plain Text**: Card nickname, issuer, last 4 digits, expiry date
- **Rationale**: Only CVC is sensitive; last 4 digits and expiry are commonly displayed

### Database File Permissions
- **Linux/macOS**: chmod 600 (read/write for owner only)
- **Windows**: Set ACL to restrict access to current user

### Logging
- **Never log**:
  - Plain-text CVC
  - Encryption keys
  - Full card numbers (even if provided by user)
- **Safe to log**:
  - Card ID
  - Nickname
  - Last 4 digits
  - Expiry date

---

## Testing Strategy

### Unit Tests
- **Database Operations**: Test insert, update, delete, select with in-memory SQLite (`:memory:`)
- **Encryption/Decryption**: Test round-trip with known inputs
- **Duplicate Detection**: Test various combinations of duplicates

### Integration Tests
- **End-to-End**: Create card with CVC → Save → Load → Decrypt → Verify
- **Migration**: Test schema upgrades from v1 to v2 (future)

### Test Data
```cpp
PaymentCard createTestCard() {
    PaymentCard card;
    card.setNickname("Test Card");
    card.setIssuer("Test Bank");
    card.setLast4Digits("1234");
    card.setExpiryDate(QDate(2028, 12, 31));
    return card;
}
```

---

## Error Codes

### Database Error Codes
| Code | Description | User Message |
|------|-------------|--------------|
| `DB_CONN_FAILED` | Cannot connect to database | "Unable to open database. Please check file permissions." |
| `DB_QUERY_FAILED` | SQL query execution failed | "Database operation failed. Please try again." |
| `DB_CONSTRAINT_VIOLATION` | Constraint check failed | "Invalid data format." |

### Encryption Error Codes
| Code | Description | User Message |
|------|-------------|--------------|
| `ENC_KEY_NOT_FOUND` | Encryption key not in keychain | "Encryption key not found. Reinitialize?" |
| `ENC_ENCRYPT_FAILED` | Encryption operation failed | "Failed to encrypt data. Please try again." |
| `ENC_DECRYPT_FAILED` | Decryption operation failed | "Unable to decrypt CVC. Please re-enter." |

---

## Dependencies

### Required Libraries
- **Qt Modules**:
  - `Qt6::Core`
  - `Qt6::Sql`
- **Encryption**:
  - `qca-qt6` (Qt Cryptographic Architecture) **OR**
  - `OpenSSL` (libssl-dev)
- **Keychain**:
  - `qt6keychain` (QtKeychain)

### Installation (Example: Ubuntu)
```bash
sudo apt-get install libqca-qt6-2-dev
sudo apt-get install qt6keychain-dev
```

### CMake Integration
```cmake
find_package(Qt6 REQUIRED COMPONENTS Core Sql)
find_package(Qca-qt6 REQUIRED)
find_package(Qt6Keychain REQUIRED)

target_link_libraries(BillOrganizer
    Qt6::Core
    Qt6::Sql
    qca-qt6
    qt6keychain
)
```

---

## Future Enhancements (Out of Scope)

- **Full Card Number Storage**: Store full card number (encrypted) instead of just last 4 digits
- **Card Verification**: Integrate with Luhn algorithm to validate card numbers
- **Cloud Sync**: Sync encrypted card data across devices
- **Multi-User Support**: Separate encryption keys per user account
- **Audit Log**: Track all card access/modifications
