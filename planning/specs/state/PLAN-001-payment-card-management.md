# Payment Card Management - State Management Specification

## Overview
This document specifies how card data is managed in memory and synchronized with the database throughout the application lifecycle. For a Qt/C++ desktop application, state management involves model classes, signals/slots, and Qt's model/view architecture.

---

## State Components

### 1. Card Data Model

**Class**: `PaymentCard` (data model)

**Properties:**
```cpp
class PaymentCard {
private:
    int id;                      // Unique identifier (primary key)
    QString nickname;            // User-defined card nickname
    QString issuer;              // Bank/issuer name
    QString last4Digits;         // Last 4 digits of card number (plain text)
    QDate expiryDate;            // Expiry date (MM/YY)
    QByteArray encryptedCVC;     // Encrypted CVC (optional, can be null)
    QDateTime createdAt;         // Timestamp when card was created
    QDateTime updatedAt;         // Timestamp when card was last updated

public:
    // Getters
    int getId() const;
    QString getNickname() const;
    QString getIssuer() const;
    QString getLast4Digits() const;
    QDate getExpiryDate() const;
    bool hasCVC() const;         // Check if CVC is stored
    QDateTime getCreatedAt() const;
    QDateTime getUpdatedAt() const;

    // Setters
    void setNickname(const QString& nickname);
    void setIssuer(const QString& issuer);
    void setLast4Digits(const QString& last4);
    void setExpiryDate(const QDate& date);
    void setEncryptedCVC(const QByteArray& cvc);
    void clearCVC();

    // Derived properties
    QString getMaskedCardNumber() const;  // Returns "•••• •••• •••• 1234"
    bool isExpired() const;               // Check if card is past expiry
    bool isExpiringSoon() const;          // Check if expiring within 30 days
    QString getExpiryStatus() const;      // Returns "Expired", "Expiring Soon", or ""
};
```

---

### 2. Card Manager (Business Logic Layer)

**Class**: `PaymentCardManager` (singleton or service class)

**Responsibilities:**
- CRUD operations for cards
- Encryption/decryption of CVC
- Validation logic
- Database interaction
- Signal emission for state changes

**Interface:**
```cpp
class PaymentCardManager : public QObject {
    Q_OBJECT

public:
    static PaymentCardManager* instance();

    // CRUD operations
    QList<PaymentCard> getAllCards();
    PaymentCard getCardById(int id);
    int addCard(const PaymentCard& card, const QString& plainCVC = QString());
    bool updateCard(int id, const PaymentCard& updatedCard, const QString& plainCVC = QString());
    bool deleteCard(int id);

    // Validation
    ValidationResult validateCard(const PaymentCard& card);
    bool isDuplicate(const QString& last4, const QDate& expiry, int excludeId = -1);

    // Query operations
    QList<PaymentCard> getActiveCards();      // Non-expired cards
    QList<PaymentCard> getExpiredCards();
    int getSubscriptionCount(int cardId);     // Count subscriptions using this card

signals:
    void cardAdded(int cardId);
    void cardUpdated(int cardId);
    void cardDeleted(int cardId);
    void cardsLoaded();

private:
    PaymentCardManager(QObject* parent = nullptr);
    CardDatabase* database;
    CardEncryption* encryption;
};
```

**Validation Result Structure:**
```cpp
struct ValidationResult {
    bool isValid;
    QMap<QString, QString> errors;  // Field name -> error message
};
```

---

### 3. Card List Model (Qt Model/View)

**Class**: `PaymentCardListModel` (extends `QAbstractListModel`)

**Responsibilities:**
- Provide card data to Qt views (QListView, QTableView)
- Handle sorting and filtering
- React to state changes from PaymentCardManager

**Interface:**
```cpp
class PaymentCardListModel : public QAbstractListModel {
    Q_OBJECT

public:
    enum CardRoles {
        IdRole = Qt::UserRole + 1,
        NicknameRole,
        IssuerRole,
        Last4Role,
        MaskedNumberRole,
        ExpiryDateRole,
        ExpiryStatusRole,
        IsExpiredRole,
        IsExpiringSoonRole
    };

    PaymentCardListModel(QObject* parent = nullptr);

    // QAbstractListModel interface
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    // Custom methods
    void refresh();                          // Reload from manager
    void setFilter(const QString& searchText);
    void setSortOrder(SortOrder order);

private slots:
    void onCardAdded(int cardId);
    void onCardUpdated(int cardId);
    void onCardDeleted(int cardId);

private:
    QList<PaymentCard> cards;
    QString filterText;
};
```

---

### 4. Card Encryption Service

**Class**: `CardEncryption`

**Responsibilities:**
- Encrypt CVC before storage
- Decrypt CVC when needed
- Key management (secure key storage)

**Interface:**
```cpp
class CardEncryption {
public:
    static CardEncryption* instance();

    // Encryption operations
    QByteArray encrypt(const QString& plainText);
    QString decrypt(const QByteArray& cipherText);

    // Key management
    bool initializeKey();
    bool hasValidKey() const;

private:
    CardEncryption();
    QByteArray encryptionKey;  // AES-256 key (securely stored)
};
```

**Encryption Details:**
- **Algorithm**: AES-256-CBC
- **Key Storage**: Qt Keychain (libqtkeychain) or platform-specific secure storage
  - macOS: Keychain
  - Windows: Credential Manager
  - Linux: libsecret/KWallet
- **IV (Initialization Vector)**: Randomly generated for each encryption, prepended to ciphertext
- **Padding**: PKCS7

---

## State Transitions

### Add Card Flow

```
User Input (Add Card Form)
    ↓
Validate Input (PaymentCardManager::validateCard)
    ↓ (if valid)
Check Duplicate (PaymentCardManager::isDuplicate)
    ↓ (if user confirms or no duplicate)
Encrypt CVC (if provided) → CardEncryption::encrypt
    ↓
Save to Database → CardDatabase::insertCard
    ↓
Emit Signal → PaymentCardManager::cardAdded(cardId)
    ↓
Update Model → PaymentCardListModel::onCardAdded
    ↓
Refresh View → UI updates automatically
```

### Edit Card Flow

```
Load Card (PaymentCardManager::getCardById)
    ↓
Decrypt CVC (if exists) → Display as "•••" in UI
    ↓
User Modifies Fields
    ↓
Validate Input (PaymentCardManager::validateCard)
    ↓ (if valid)
Handle CVC Update:
  - If user entered new CVC → Encrypt new value
  - If user left as "•••" → Keep existing encrypted value
  - If user cleared field → Remove CVC from storage
    ↓
Update Database → CardDatabase::updateCard
    ↓
Emit Signal → PaymentCardManager::cardUpdated(cardId)
    ↓
Update Model → PaymentCardListModel::onCardUpdated
    ↓
Refresh View → UI updates automatically
```

### Delete Card Flow

```
User Initiates Delete
    ↓
Check Subscription Links → PaymentCardManager::getSubscriptionCount(cardId)
    ↓
Show Confirmation Dialog (with subscription warning if applicable)
    ↓ (if user confirms)
Delete from Database → CardDatabase::deleteCard
    ↓
Emit Signal → PaymentCardManager::cardDeleted(cardId)
    ↓
Update Model → PaymentCardListModel::onCardDeleted
    ↓
Refresh View → UI updates automatically
```

### Card Selection Flow

```
Subscription Form Requests Card Selection
    ↓
Open Card Selection Dialog
    ↓
Load Cards → PaymentCardManager::getActiveCards() (optionally filter expired)
    ↓
Populate Selection List → PaymentCardListModel
    ↓
User Selects Card
    ↓
Return Card ID to Subscription Form
    ↓
Close Dialog
```

---

## Data Persistence

### Database Schema

**Table**: `payment_cards`

| Column | Type | Constraints | Description |
|--------|------|-------------|-------------|
| id | INTEGER | PRIMARY KEY AUTOINCREMENT | Unique card identifier |
| nickname | TEXT | NOT NULL | User-defined nickname |
| issuer | TEXT | NOT NULL | Bank/issuer name |
| last4_digits | TEXT | NOT NULL | Last 4 digits (plain text) |
| expiry_month | INTEGER | NOT NULL | Expiry month (1-12) |
| expiry_year | INTEGER | NOT NULL | Expiry year (YYYY) |
| encrypted_cvc | BLOB | NULL | Encrypted CVC (optional) |
| created_at | TEXT | NOT NULL DEFAULT CURRENT_TIMESTAMP | Creation timestamp |
| updated_at | TEXT | NOT NULL DEFAULT CURRENT_TIMESTAMP | Last update timestamp |

**Indexes:**
- `idx_last4_expiry` on (last4_digits, expiry_month, expiry_year) for duplicate detection
- `idx_created_at` on (created_at) for sorting

---

## Validation Rules

### Card Nickname
- **Required**: Yes
- **Min Length**: 1 character
- **Max Length**: 50 characters
- **Allowed Characters**: Any (including spaces, emojis)

### Issuer/Bank
- **Required**: Yes
- **Min Length**: 1 character
- **Max Length**: 50 characters
- **Allowed Characters**: Any

### Last 4 Digits
- **Required**: Yes
- **Format**: Exactly 4 numeric digits
- **Validation Regex**: `^[0-9]{4}$`

### Expiry Date
- **Required**: Yes
- **Format**: MM/YY
- **Validation**:
  - Month must be 01-12
  - Year must be 2-digit (YY)
  - Combined date must be in the future (greater than current month/year)
- **Parsing**: Convert MM/YY to QDate(2000+YY, MM, last day of month)

### CVC
- **Required**: No (optional)
- **Format**: 3-4 numeric digits
- **Validation Regex**: `^[0-9]{3,4}$`
- **Storage**: Encrypted with AES-256

---

## Error Handling

### Database Errors
- **Connection Failure**: Show error dialog, retry mechanism
- **Write Failure**: Log error, show user-friendly message, do not navigate away
- **Constraint Violation**: Handle gracefully (e.g., unique constraint on card details)

### Encryption Errors
- **Key Not Found**: Prompt user to reinitialize key (warning: existing encrypted data will be lost)
- **Decryption Failure**: Log error, show generic "Unable to decrypt" message, allow user to update CVC

### Validation Errors
- **Field-Level**: Show inline error messages below each field
- **Form-Level**: Prevent submission until all errors resolved
- **Duplicate Detection**: Show confirmation dialog with existing card details

---

## Memory Management

### Qt Object Lifecycle
- `PaymentCardManager`: Singleton, lives for application lifetime
- `PaymentCardListModel`: Owned by view, destroyed when view closes
- `PaymentCard`: Value objects, copied by value or shared via QSharedPointer

### Sensitive Data Handling
- **CVC in Memory**:
  - Store only encrypted form in `PaymentCard` objects
  - Decrypt only when needed for display or update
  - Clear plain-text CVC from memory immediately after use
- **Encryption Key**:
  - Load from secure storage on app startup
  - Keep in memory (in `CardEncryption` singleton)
  - Never log or write to disk unencrypted

---

## Signals and Slots

### PaymentCardManager Signals
```cpp
signals:
    void cardAdded(int cardId);        // Emitted after successful card creation
    void cardUpdated(int cardId);      // Emitted after successful card update
    void cardDeleted(int cardId);      // Emitted after successful card deletion
    void cardsLoaded();                // Emitted after bulk load (e.g., app startup)
    void operationFailed(QString error); // Emitted on any operation failure
```

### PaymentCardListModel Slots
```cpp
private slots:
    void onCardAdded(int cardId);      // Refresh model, insert new row
    void onCardUpdated(int cardId);    // Refresh specific row
    void onCardDeleted(int cardId);    // Remove row from model
```

### UI Slots
```cpp
// Card List View
private slots:
    void onAddCardClicked();
    void onEditCardClicked(int cardId);
    void onDeleteCardClicked(int cardId);
    void onSearchTextChanged(QString text);

// Card Form
private slots:
    void onSaveClicked();
    void onCancelClicked();
    void onFieldChanged();             // Enable/disable Save button based on validation
```

---

## Testing Considerations

### Unit Tests
- `PaymentCard`: Test getters, setters, derived properties
- `PaymentCardManager`: Test CRUD operations with mock database
- `CardEncryption`: Test encrypt/decrypt round-trip, key initialization
- Validation logic: Test all validation rules with valid/invalid inputs

### Integration Tests
- End-to-end card creation with encryption
- Card update with CVC preservation
- Card deletion with subscription check
- Duplicate detection across database

### UI Tests
- Form validation feedback
- State transitions (list → add → save → list)
- Error message display
- Card selection dialog flow

---

## Performance Considerations

### Database Queries
- Use prepared statements for all queries
- Load cards lazily (only when card list view is opened)
- Cache card list in model until state changes

### Encryption
- Encrypt/decrypt asynchronously to avoid UI blocking
- Use Qt's `QFuture` and `QtConcurrent` for background operations

### UI Responsiveness
- Update model incrementally (insert/update/delete single rows) instead of full refresh
- Use virtual scrolling for large card lists (Qt handles this automatically with models)

---

## Security Notes

### Threat Model
- **Threat**: Local attacker with file system access
- **Mitigation**: Encrypt CVC at rest, use OS-level secure storage for encryption key

### Not Protected Against
- Memory dumping attacks (CVC briefly in plain text during display/update)
- Malware with elevated privileges
- Physical access to unlocked device

### Recommendations
- Do not log card details or CVC (encrypted or plain)
- Clear sensitive data from memory after use
- Use Qt's secure input widgets (password fields) for CVC
- Consider auto-lock after inactivity for additional protection

---

## Future Enhancements (Out of Scope for PLAN-001)

- **Card Images**: Store card issuer logos or custom images
- **Card Categories**: Tag cards (personal, business, family)
- **Card Statistics**: Track which cards are used most for subscriptions
- **Export/Import**: Secure backup and restore of card data
- **Multi-Factor Protection**: Require password/biometric to view/edit cards
