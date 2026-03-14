# Payment Card Management - Unit Test Cases

## Overview
Unit tests for individual components and functions in the payment card management system.

---

## 1. PaymentCard Model Tests

### 1.1 Getters and Setters

| ID | Scenario | Input | Expected Result |
|----|----------|-------|-----------------|
| TC-UNIT-001 | Set and get nickname | setNickname("My Visa") | getNickname() returns "My Visa" |
| TC-UNIT-002 | Set and get issuer | setIssuer("Chase") | getIssuer() returns "Chase" |
| TC-UNIT-003 | Set and get last 4 digits | setLast4Digits("1234") | getLast4Digits() returns "1234" |
| TC-UNIT-004 | Set and get expiry date | setExpiryDate(QDate(2028, 12, 1)) | getExpiryDate() returns QDate(2028, 12, 1) |
| TC-UNIT-005 | Set and get encrypted CVC | setEncryptedCVC(QByteArray::fromHex("abc123")) | hasCVC() returns true |
| TC-UNIT-006 | Clear CVC | clearCVC() after setting CVC | hasCVC() returns false |

### 1.2 Derived Properties - getMaskedCardNumber()

| ID | Scenario | Input | Expected Result |
|----|----------|-------|-----------------|
| TC-UNIT-010 | Mask card with last 4 digits "1234" | setLast4Digits("1234") | getMaskedCardNumber() returns "•••• •••• •••• 1234" |
| TC-UNIT-011 | Mask card with last 4 digits "0000" | setLast4Digits("0000") | getMaskedCardNumber() returns "•••• •••• •••• 0000" |
| TC-UNIT-012 | Mask card with last 4 digits "9999" | setLast4Digits("9999") | getMaskedCardNumber() returns "•••• •••• •••• 9999" |

### 1.3 Derived Properties - isExpired()

| ID | Scenario | Input | Expected Result |
|----|----------|-------|-----------------|
| TC-UNIT-020 | Card expiring in future | setExpiryDate(QDate(2030, 12, 1)), current date = 2026-03-13 | isExpired() returns false |
| TC-UNIT-021 | Card expired in past | setExpiryDate(QDate(2025, 1, 1)), current date = 2026-03-13 | isExpired() returns true |
| TC-UNIT-022 | Card expiring current month | setExpiryDate(QDate(2026, 3, 1)), current date = 2026-03-13 | isExpired() returns false |
| TC-UNIT-023 | Card expiring next month | setExpiryDate(QDate(2026, 4, 1)), current date = 2026-03-13 | isExpired() returns false |

### 1.4 Derived Properties - isExpiringSoon()

| ID | Scenario | Input | Expected Result |
|----|----------|-------|-----------------|
| TC-UNIT-030 | Card expiring in 29 days | setExpiryDate(29 days from now) | isExpiringSoon() returns true |
| TC-UNIT-031 | Card expiring in 30 days | setExpiryDate(30 days from now) | isExpiringSoon() returns true |
| TC-UNIT-032 | Card expiring in 31 days | setExpiryDate(31 days from now) | isExpiringSoon() returns false |
| TC-UNIT-033 | Card expiring in 90 days | setExpiryDate(90 days from now) | isExpiringSoon() returns false |
| TC-UNIT-034 | Card already expired | setExpiryDate(past date) | isExpiringSoon() returns false (expired, not expiring) |

### 1.5 Derived Properties - getExpiryStatus()

| ID | Scenario | Input | Expected Result |
|----|----------|-------|-----------------|
| TC-UNIT-040 | Card expired | setExpiryDate(past date) | getExpiryStatus() returns "Expired" |
| TC-UNIT-041 | Card expiring within 30 days | setExpiryDate(20 days from now) | getExpiryStatus() returns "Expiring Soon" |
| TC-UNIT-042 | Card valid and not expiring soon | setExpiryDate(60 days from now) | getExpiryStatus() returns "" (empty string) |

---

## 2. CardEncryption Service Tests

### 2.1 Encryption Operations

| ID | Scenario | Input | Expected Result |
|----|----------|-------|-----------------|
| TC-UNIT-050 | Encrypt plain CVC "123" | encrypt("123") | Returns non-empty QByteArray with length >= 32 bytes (IV + ciphertext) |
| TC-UNIT-051 | Encrypt plain CVC "1234" | encrypt("1234") | Returns non-empty QByteArray with length >= 32 bytes |
| TC-UNIT-052 | Encrypt empty string | encrypt("") | Returns QByteArray (handles gracefully) |
| TC-UNIT-053 | Encrypt special characters | encrypt("!@#$") | Returns non-empty QByteArray |
| TC-UNIT-054 | Two encryptions produce different results | encrypt("123") twice | Two different ciphertexts (due to random IV) |

### 2.2 Decryption Operations

| ID | Scenario | Input | Expected Result |
|----|----------|-------|-----------------|
| TC-UNIT-060 | Decrypt valid ciphertext "123" | decrypt(encrypt("123")) | Returns "123" |
| TC-UNIT-061 | Decrypt valid ciphertext "1234" | decrypt(encrypt("1234")) | Returns "1234" |
| TC-UNIT-062 | Decrypt empty ciphertext | decrypt(QByteArray()) | Handles gracefully (returns empty or throws specific error) |
| TC-UNIT-063 | Decrypt corrupted ciphertext | decrypt(corrupted bytes) | Handles gracefully (returns empty or throws DecryptionError) |

### 2.3 Round-Trip Encryption/Decryption

| ID | Scenario | Input | Expected Result |
|----|----------|-------|-----------------|
| TC-UNIT-070 | Round-trip "123" | decrypt(encrypt("123")) | Returns "123" |
| TC-UNIT-071 | Round-trip "1234" | decrypt(encrypt("1234")) | Returns "1234" |
| TC-UNIT-072 | Round-trip "000" | decrypt(encrypt("000")) | Returns "000" |
| TC-UNIT-073 | Round-trip "9999" | decrypt(encrypt("9999")) | Returns "9999" |
| TC-UNIT-074 | Round-trip Unicode characters | decrypt(encrypt("€£¥")) | Returns "€£¥" |

### 2.4 Key Management

| ID | Scenario | Action | Expected Result |
|----|----------|--------|-----------------|
| TC-UNIT-080 | Initialize encryption key on first run | initializeKey() | Returns true, hasValidKey() returns true |
| TC-UNIT-081 | Check valid key exists | hasValidKey() after initialization | Returns true |
| TC-UNIT-082 | Encryption fails without key | encrypt("123") before initializeKey() | Throws error or returns empty (graceful failure) |

---

## 3. PaymentCardManager Validation Tests

### 3.1 Validate Nickname

| ID | Scenario | Input | Expected Result |
|----|----------|-------|-----------------|
| TC-UNIT-100 | Valid nickname "My Visa" | nickname = "My Visa" | ValidationResult.isValid = true, no errors |
| TC-UNIT-101 | Valid nickname with 50 characters | nickname = "A" * 50 | ValidationResult.isValid = true |
| TC-UNIT-102 | Empty nickname | nickname = "" | ValidationResult.isValid = false, errors["nickname"] = "Card Nickname is required" |
| TC-UNIT-103 | Nickname exceeds 50 characters | nickname = "A" * 51 | ValidationResult.isValid = false, errors["nickname"] = "Card Nickname must not exceed 50 characters" |
| TC-UNIT-104 | Nickname with emojis | nickname = "My Card 💳" | ValidationResult.isValid = true |
| TC-UNIT-105 | Nickname with spaces only | nickname = "   " | ValidationResult.isValid = false, errors["nickname"] = "Card Nickname is required" |

### 3.2 Validate Issuer

| ID | Scenario | Input | Expected Result |
|----|----------|-------|-----------------|
| TC-UNIT-110 | Valid issuer "Chase" | issuer = "Chase" | ValidationResult.isValid = true, no errors |
| TC-UNIT-111 | Valid issuer with 50 characters | issuer = "A" * 50 | ValidationResult.isValid = true |
| TC-UNIT-112 | Empty issuer | issuer = "" | ValidationResult.isValid = false, errors["issuer"] = "Issuer/Bank is required" |
| TC-UNIT-113 | Issuer exceeds 50 characters | issuer = "A" * 51 | ValidationResult.isValid = false, errors["issuer"] = "Issuer/Bank must not exceed 50 characters" |
| TC-UNIT-114 | Issuer with spaces only | issuer = "   " | ValidationResult.isValid = false, errors["issuer"] = "Issuer/Bank is required" |

### 3.3 Validate Last 4 Digits

| ID | Scenario | Input | Expected Result |
|----|----------|-------|-----------------|
| TC-UNIT-120 | Valid last 4 digits "1234" | last4 = "1234" | ValidationResult.isValid = true, no errors |
| TC-UNIT-121 | Valid last 4 digits "0000" | last4 = "0000" | ValidationResult.isValid = true |
| TC-UNIT-122 | Valid last 4 digits "9999" | last4 = "9999" | ValidationResult.isValid = true |
| TC-UNIT-123 | Empty last 4 digits | last4 = "" | ValidationResult.isValid = false, errors["last4"] = "Last 4 Digits is required" |
| TC-UNIT-124 | Last 4 digits with 3 characters | last4 = "123" | ValidationResult.isValid = false, errors["last4"] = "Must be exactly 4 digits" |
| TC-UNIT-125 | Last 4 digits with 5 characters | last4 = "12345" | ValidationResult.isValid = false, errors["last4"] = "Must be exactly 4 digits" |
| TC-UNIT-126 | Last 4 digits with letters | last4 = "12ab" | ValidationResult.isValid = false, errors["last4"] = "Must be exactly 4 digits" |
| TC-UNIT-127 | Last 4 digits with special chars | last4 = "12@#" | ValidationResult.isValid = false, errors["last4"] = "Must be exactly 4 digits" |
| TC-UNIT-128 | Last 4 digits with spaces | last4 = "12 34" | ValidationResult.isValid = false, errors["last4"] = "Must be exactly 4 digits" |

### 3.4 Validate Expiry Date

| ID | Scenario | Input | Expected Result |
|----|----------|-------|-----------------|
| TC-UNIT-130 | Valid future date | expiryDate = QDate(2028, 12, 1) | ValidationResult.isValid = true, no errors |
| TC-UNIT-131 | Valid date next month | expiryDate = next month's date | ValidationResult.isValid = true |
| TC-UNIT-132 | Current month | expiryDate = current month | ValidationResult.isValid = true |
| TC-UNIT-133 | Past date | expiryDate = QDate(2025, 1, 1) | ValidationResult.isValid = false, errors["expiry"] = "Expiry date must be in the future" |
| TC-UNIT-134 | Invalid month 0 | expiryDate = QDate(2028, 0, 1) | ValidationResult.isValid = false, errors["expiry"] = "Please enter a valid expiry date" |
| TC-UNIT-135 | Invalid month 13 | expiryDate = QDate(2028, 13, 1) | ValidationResult.isValid = false, errors["expiry"] = "Please enter a valid expiry date" |
| TC-UNIT-136 | Null date | expiryDate = QDate() | ValidationResult.isValid = false, errors["expiry"] = "Expiry Date is required" |

### 3.5 Validate CVC (Optional)

| ID | Scenario | Input | Expected Result |
|----|----------|-------|-----------------|
| TC-UNIT-140 | No CVC provided | cvc = "" | ValidationResult.isValid = true (optional field) |
| TC-UNIT-141 | Valid 3-digit CVC | cvc = "123" | ValidationResult.isValid = true |
| TC-UNIT-142 | Valid 4-digit CVC | cvc = "1234" | ValidationResult.isValid = true |
| TC-UNIT-143 | Invalid 2-digit CVC | cvc = "12" | ValidationResult.isValid = false, errors["cvc"] = "CVC must be 3-4 digits" |
| TC-UNIT-144 | Invalid 5-digit CVC | cvc = "12345" | ValidationResult.isValid = false, errors["cvc"] = "CVC must be 3-4 digits" |
| TC-UNIT-145 | CVC with letters | cvc = "12a" | ValidationResult.isValid = false, errors["cvc"] = "CVC must be 3-4 digits" |
| TC-UNIT-146 | CVC with special chars | cvc = "12@" | ValidationResult.isValid = false, errors["cvc"] = "CVC must be 3-4 digits" |

### 3.6 Complete Card Validation

| ID | Scenario | Input | Expected Result |
|----|----------|-------|-----------------|
| TC-UNIT-150 | All valid fields without CVC | nickname="My Visa", issuer="Chase", last4="1234", expiry=future date, cvc="" | ValidationResult.isValid = true, no errors |
| TC-UNIT-151 | All valid fields with CVC | nickname="My Visa", issuer="Chase", last4="1234", expiry=future date, cvc="123" | ValidationResult.isValid = true, no errors |
| TC-UNIT-152 | Multiple invalid fields | nickname="", issuer="", last4="", expiry=null | ValidationResult.isValid = false, multiple errors in errors map |
| TC-UNIT-153 | One invalid field | All valid except last4="123" | ValidationResult.isValid = false, errors["last4"] present |

---

## 4. PaymentCardManager Duplicate Detection Tests

### 4.1 isDuplicate() Tests

| ID | Scenario | Input | Expected Result |
|----|----------|-------|-----------------|
| TC-UNIT-160 | No existing cards | last4="1234", expiry=12/28 | isDuplicate() returns false |
| TC-UNIT-161 | Exact duplicate exists | last4="1234", expiry=12/28 (card already in DB) | isDuplicate() returns true |
| TC-UNIT-162 | Same last4, different expiry | last4="1234", expiry=11/28 (DB has 1234/12/28) | isDuplicate() returns false |
| TC-UNIT-163 | Different last4, same expiry | last4="5678", expiry=12/28 (DB has 1234/12/28) | isDuplicate() returns false |
| TC-UNIT-164 | Exclude current card during update | last4="1234", expiry=12/28, excludeId=5 (card 5 has 1234/12/28) | isDuplicate() returns false |
| TC-UNIT-165 | Exclude current card but other duplicate exists | last4="1234", expiry=12/28, excludeId=5 (card 3 and 5 both have 1234/12/28) | isDuplicate() returns true |

---

## 5. CardDatabase Tests

### 5.1 Database Initialization

| ID | Scenario | Action | Expected Result |
|----|----------|--------|-----------------|
| TC-UNIT-200 | Initialize with valid path | initialize("/tmp/test.db") | Returns true, database file created |
| TC-UNIT-201 | Create tables on first run | createTables() | Returns true, payment_cards table exists |
| TC-UNIT-202 | Create tables when already exist | createTables() twice | Returns true both times (idempotent) |
| TC-UNIT-203 | Initialize with invalid path | initialize("/invalid/path/test.db") | Returns false or throws error |

### 5.2 Row Mapping

| ID | Scenario | Input | Expected Result |
|----|----------|-------|-----------------|
| TC-UNIT-210 | Map database row to PaymentCard | DB row with all fields | PaymentCard object with matching properties |
| TC-UNIT-211 | Map row with NULL encrypted_cvc | DB row with encrypted_cvc = NULL | PaymentCard with hasCVC() = false |
| TC-UNIT-212 | Map row with encrypted_cvc | DB row with encrypted_cvc = blob | PaymentCard with hasCVC() = true |
| TC-UNIT-213 | Map expiry date correctly | DB row with expiry_month=12, expiry_year=2028 | PaymentCard.getExpiryDate() = QDate(2028, 12, 1) |

---

## 6. Edge Cases and Error Handling Tests

### 6.1 Memory Management

| ID | Scenario | Action | Expected Result |
|----|----------|--------|-----------------|
| TC-UNIT-300 | Clear CVC from memory after encryption | Encrypt CVC, check plain text cleared | Plain CVC string no longer in memory |
| TC-UNIT-301 | PaymentCard copy constructor | PaymentCard a; PaymentCard b(a); | Deep copy, independent objects |
| TC-UNIT-302 | PaymentCard assignment operator | PaymentCard a; PaymentCard b; b = a; | Deep copy, independent objects |

### 6.2 Boundary Conditions

| ID | Scenario | Input | Expected Result |
|----|----------|-------|-----------------|
| TC-UNIT-310 | Nickname exactly 50 characters | nickname with length 50 | ValidationResult.isValid = true |
| TC-UNIT-311 | Issuer exactly 50 characters | issuer with length 50 | ValidationResult.isValid = true |
| TC-UNIT-312 | Expiry date at month boundary | expiryDate = last day of month | Validation passes |
| TC-UNIT-313 | Expiry date at year boundary | expiryDate = QDate(2099, 12, 31) | Validation passes |

### 6.3 Null and Empty Handling

| ID | Scenario | Input | Expected Result |
|----|----------|-------|-----------------|
| TC-UNIT-320 | Null QString for nickname | setNickname(QString()) | Validation fails (required field) |
| TC-UNIT-321 | Null QDate for expiry | setExpiryDate(QDate()) | Validation fails (required field) |
| TC-UNIT-322 | Empty QByteArray for encrypted CVC | setEncryptedCVC(QByteArray()) | hasCVC() returns false |

---

## Test Environment Setup

### Required Test Framework
- **Framework**: Qt Test (QTest)
- **Test Type**: Unit tests with mock database (`:memory:` SQLite)
- **Coverage Tool**: gcov/lcov or Qt Creator's code coverage

### Mock Dependencies
- **Database**: Use in-memory SQLite (`:memory:`)
- **Encryption Key**: Use fixed test key (not from keychain)
- **Current Date**: Use injectable clock for date-dependent tests

### Test Data Builders
```cpp
// Example test helper
PaymentCard createValidTestCard() {
    PaymentCard card;
    card.setNickname("Test Card");
    card.setIssuer("Test Bank");
    card.setLast4Digits("1234");
    card.setExpiryDate(QDate::currentDate().addMonths(12));
    return card;
}
```

---

## Success Criteria

- **All unit tests pass**: 100% pass rate
- **Code coverage**: Minimum 80% line coverage for business logic
- **No memory leaks**: Valgrind clean run
- **Fast execution**: All unit tests complete in < 5 seconds
