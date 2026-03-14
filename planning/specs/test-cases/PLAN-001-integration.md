# Payment Card Management - Integration Test Cases

## Overview
Integration tests for the payment card management system, testing interactions between components (CardManager + Database + Encryption).

---

## 1. Add Card Integration Tests

### 1.1 Add Card - Normal Cases

| ID | Scenario | Action | Expected Result |
|----|----------|--------|-----------------|
| TC-INT-001 | Add card without CVC | Create card with nickname="My Visa", issuer="Chase", last4="1234", expiry=12/28, no CVC → PaymentCardManager.addCard() | Card saved to database, cardAdded signal emitted, returns card ID > 0 |
| TC-INT-002 | Add card with CVC | Create card with CVC="123" → PaymentCardManager.addCard() | Card saved, CVC encrypted and stored, cardAdded signal emitted |
| TC-INT-003 | Add multiple cards | Add 3 different cards sequentially | All cards saved, getAllCards() returns 3 cards |
| TC-INT-004 | Add card and verify database persistence | Add card → Close database → Reopen → getAllCards() | Previously added card still present |
| TC-INT-005 | Add card with 4-digit CVC | Create card with CVC="1234" → addCard() | CVC encrypted correctly (blob length >= 32 bytes) |

### 1.2 Add Card - Duplicate Detection

| ID | Scenario | Action | Expected Result |
|----|----------|--------|-----------------|
| TC-INT-010 | Add duplicate card | Add card with last4="1234", expiry=12/28 → Add another with same details | isDuplicate() returns true before second save |
| TC-INT-011 | Add card with same last4 but different expiry | Add 1234/12/28 → Add 1234/11/28 | Both cards saved (not duplicates) |
| TC-INT-012 | Add card with different last4 but same expiry | Add 1234/12/28 → Add 5678/12/28 | Both cards saved (not duplicates) |

### 1.3 Add Card - Encryption Verification

| ID | Scenario | Action | Expected Result |
|----|----------|--------|-----------------|
| TC-INT-020 | Verify CVC encryption at rest | Add card with CVC="123" → Query database directly | encrypted_cvc column contains encrypted bytes, not plain "123" |
| TC-INT-021 | Verify CVC not logged | Add card with CVC="123", check logs | CVC "123" does not appear in any log files |
| TC-INT-022 | Verify encrypted CVC differs on each add | Add card with CVC="123" → Delete → Add again with CVC="123" | Two different encrypted blobs (different IVs) |

### 1.4 Add Card - Error Cases

| ID | Scenario | Action | Expected Result |
|----|----------|--------|-----------------|
| TC-INT-030 | Add card with invalid data | addCard() with empty nickname | Validation fails, card not saved, operationFailed signal emitted |
| TC-INT-031 | Add card with database connection closed | Close database → addCard() | Returns error, operationFailed signal emitted |
| TC-INT-032 | Add card without encryption key initialized | addCard() with CVC before initializeKey() | Encryption fails, card not saved or CVC not stored |

---

## 2. Edit Card Integration Tests

### 2.1 Edit Card - Normal Cases

| ID | Scenario | Action | Expected Result |
|----|----------|--------|-----------------|
| TC-INT-050 | Edit card nickname | Add card → Update nickname from "My Visa" to "Personal Visa" → updateCard() | Card updated in database, cardUpdated signal emitted |
| TC-INT-051 | Edit card issuer | Add card → Update issuer from "Chase" to "Bank of America" | Card updated, database reflects change |
| TC-INT-052 | Edit card last 4 digits | Add card → Update last4 from "1234" to "5678" | Card updated, database reflects change |
| TC-INT-053 | Edit card expiry date | Add card → Update expiry from 12/28 to 06/29 | Card updated, database reflects change |
| TC-INT-054 | Edit multiple fields at once | Add card → Update nickname, issuer, last4, expiry | All fields updated correctly |

### 2.2 Edit Card - CVC Update Cases

| ID | Scenario | Action | Expected Result |
|----|----------|--------|-----------------|
| TC-INT-060 | Add new CVC to card without CVC | Add card without CVC → Edit and add CVC="123" | CVC encrypted and saved, hasCVC() = true |
| TC-INT-061 | Update existing CVC | Add card with CVC="123" → Edit and change to CVC="456" | Old CVC replaced with encrypted "456" |
| TC-INT-062 | Remove CVC from card | Add card with CVC="123" → Edit and clear CVC (pass empty string) | encrypted_cvc set to NULL in database, hasCVC() = false |
| TC-INT-063 | Keep existing CVC unchanged | Add card with CVC="123" → Edit other fields, don't modify CVC | Original encrypted CVC preserved in database |
| TC-INT-064 | Verify CVC update encryption | Update CVC from "123" to "456" → Query database | encrypted_cvc blob changed to new encrypted value |

### 2.3 Edit Card - Duplicate Detection

| ID | Scenario | Action | Expected Result |
|----|----------|--------|-----------------|
| TC-INT-070 | Edit card to match existing card details | Add card A (1234/12/28) and card B (5678/11/28) → Edit B to 1234/12/28 | isDuplicate() returns true (excluding card B's ID) |
| TC-INT-071 | Edit card without creating duplicate | Add card A (1234/12/28) → Edit A's nickname only | No duplicate detected (excludes own ID) |

### 2.4 Edit Card - Error Cases

| ID | Scenario | Action | Expected Result |
|----|----------|--------|-----------------|
| TC-INT-080 | Edit non-existent card | updateCard() with invalid ID | Returns false, operationFailed signal emitted |
| TC-INT-081 | Edit card with invalid data | updateCard() with empty issuer | Validation fails, card not updated |
| TC-INT-082 | Edit card with database connection closed | Close database → updateCard() | Returns error, operationFailed signal emitted |

---

## 3. Delete Card Integration Tests

### 3.1 Delete Card - Normal Cases

| ID | Scenario | Action | Expected Result |
|----|----------|--------|-----------------|
| TC-INT-100 | Delete card without subscriptions | Add card → deleteCard(cardId) | Card removed from database, cardDeleted signal emitted |
| TC-INT-101 | Delete card and verify persistence | Add card → Delete → Close database → Reopen → getAllCards() | Deleted card not present |
| TC-INT-102 | Delete one of multiple cards | Add 3 cards → Delete card #2 | Card #2 removed, cards #1 and #3 remain, getAllCards() returns 2 cards |
| TC-INT-103 | Delete card with CVC | Add card with CVC → Delete | Card and encrypted CVC completely removed from database |

### 3.2 Delete Card - Subscription Link Cases

| ID | Scenario | Action | Expected Result |
|----|----------|--------|-----------------|
| TC-INT-110 | Check subscription count before delete | Add card → Link to 2 subscriptions → getSubscriptionCount(cardId) | Returns 2 |
| TC-INT-111 | Delete card linked to subscriptions | Add card → Link to subscriptions → deleteCard() | Card deleted, subscription.payment_card_id set to NULL (not cascade delete) |
| TC-INT-112 | Delete card with no subscriptions | Add card (no subscriptions) → getSubscriptionCount(cardId) | Returns 0 |

### 3.3 Delete Card - Error Cases

| ID | Scenario | Action | Expected Result |
|----|----------|--------|-----------------|
| TC-INT-120 | Delete non-existent card | deleteCard() with invalid ID | Returns false, operationFailed signal emitted |
| TC-INT-121 | Delete card with database connection closed | Close database → deleteCard() | Returns error, operationFailed signal emitted |
| TC-INT-122 | Delete card twice | Add card → deleteCard(id) → deleteCard(id) again | Second delete returns false (card not found) |

---

## 4. Load and Query Integration Tests

### 4.1 Load Cards

| ID | Scenario | Action | Expected Result |
|----|----------|--------|-----------------|
| TC-INT-150 | Load all cards from empty database | getAllCards() with no cards in DB | Returns empty QList<PaymentCard> |
| TC-INT-151 | Load all cards | Add 5 cards → getAllCards() | Returns QList with 5 PaymentCard objects |
| TC-INT-152 | Load cards in correct order | Add cards with different created_at timestamps → getAllCards() | Cards ordered by created_at DESC (newest first) |
| TC-INT-153 | Load card by ID | Add card → getCardById(cardId) | Returns correct PaymentCard object |
| TC-INT-154 | Load card by invalid ID | getCardById(999) with no such card | Returns null/empty PaymentCard or throws error |

### 4.2 Load Cards with Encryption

| ID | Scenario | Action | Expected Result |
|----|----------|--------|-----------------|
| TC-INT-160 | Load card with CVC | Add card with CVC="123" → getCardById(id) | PaymentCard returned with hasCVC() = true (encrypted CVC loaded) |
| TC-INT-161 | Load card without CVC | Add card without CVC → getCardById(id) | PaymentCard returned with hasCVC() = false |
| TC-INT-162 | Load cards after key rotation | Add cards → Rotate encryption key → getAllCards() | Old cards fail decryption gracefully (show "Unable to decrypt" message) |

### 4.3 Query Active/Expired Cards

| ID | Scenario | Action | Expected Result |
|----|----------|--------|-----------------|
| TC-INT-170 | Get active cards only | Add 3 active cards + 2 expired → getActiveCards() | Returns 3 active cards |
| TC-INT-171 | Get expired cards only | Add 3 active cards + 2 expired → getExpiredCards() | Returns 2 expired cards |
| TC-INT-172 | Get active cards when all expired | Add 5 expired cards → getActiveCards() | Returns empty list |

---

## 5. Encryption Integration Tests

### 5.1 Key Initialization

| ID | Scenario | Action | Expected Result |
|----|----------|--------|-----------------|
| TC-INT-200 | Initialize key on first run | CardEncryption.initializeKey() | Key generated, stored in keychain, hasValidKey() = true |
| TC-INT-201 | Load existing key from keychain | Initialize key → Restart app → Load key | Same key loaded from keychain |
| TC-INT-202 | Handle missing key in keychain | Delete key from keychain → loadKey() | Returns error or generates new key with warning |

### 5.2 Encryption Round-Trip with Database

| ID | Scenario | Action | Expected Result |
|----|----------|--------|-----------------|
| TC-INT-210 | Full encryption flow for CVC "123" | Encrypt "123" → Store in DB → Load from DB → Decrypt | Decrypted value = "123" |
| TC-INT-211 | Full encryption flow for CVC "1234" | Encrypt "1234" → Store → Load → Decrypt | Decrypted value = "1234" |
| TC-INT-212 | Multiple cards with same CVC value | Add 2 cards both with CVC="123" | Both have different encrypted blobs (different IVs) |

### 5.3 Decryption Error Handling

| ID | Scenario | Action | Expected Result |
|----|----------|--------|-----------------|
| TC-INT-220 | Decrypt with wrong key | Encrypt CVC with key A → Decrypt with key B | Decryption fails gracefully, returns error |
| TC-INT-221 | Decrypt corrupted ciphertext | Corrupt encrypted_cvc in database → Load card | Decryption fails, error logged, user notified |
| TC-INT-222 | Decrypt empty ciphertext | Store NULL in encrypted_cvc → Load card | hasCVC() = false, no decryption attempted |

---

## 6. Signal and Slot Integration Tests

### 6.1 cardAdded Signal

| ID | Scenario | Action | Expected Result |
|----|----------|--------|-----------------|
| TC-INT-250 | cardAdded emitted on successful add | Connect slot to cardAdded → addCard() | Slot receives cardAdded(cardId) with correct ID |
| TC-INT-251 | cardAdded includes correct card ID | Connect slot → addCard() → Check ID parameter | ID matches getCardById() result |
| TC-INT-252 | cardAdded not emitted on failure | Connect slot → addCard() with invalid data | cardAdded not emitted, operationFailed emitted instead |

### 6.2 cardUpdated Signal

| ID | Scenario | Action | Expected Result |
|----|----------|--------|-----------------|
| TC-INT-260 | cardUpdated emitted on successful update | Connect slot to cardUpdated → updateCard() | Slot receives cardUpdated(cardId) with correct ID |
| TC-INT-261 | cardUpdated not emitted on failure | Connect slot → updateCard() with invalid data | cardUpdated not emitted |

### 6.3 cardDeleted Signal

| ID | Scenario | Action | Expected Result |
|----|----------|--------|-----------------|
| TC-INT-270 | cardDeleted emitted on successful delete | Connect slot to cardDeleted → deleteCard() | Slot receives cardDeleted(cardId) with correct ID |
| TC-INT-271 | cardDeleted not emitted on failure | Connect slot → deleteCard() with invalid ID | cardDeleted not emitted |

### 6.4 operationFailed Signal

| ID | Scenario | Action | Expected Result |
|----|----------|--------|-----------------|
| TC-INT-280 | operationFailed emitted on add failure | Connect slot → addCard() with DB closed | Slot receives operationFailed(errorMessage) |
| TC-INT-281 | operationFailed emitted on update failure | Connect slot → updateCard() with invalid ID | Slot receives operationFailed(errorMessage) |
| TC-INT-282 | operationFailed emitted on delete failure | Connect slot → deleteCard() with DB closed | Slot receives operationFailed(errorMessage) |

---

## 7. PaymentCardListModel Integration Tests

### 7.1 Model Refresh on State Changes

| ID | Scenario | Action | Expected Result |
|----|----------|--------|-----------------|
| TC-INT-300 | Model updates on cardAdded | Connect model to manager → addCard() | Model emits dataChanged, rowCount increases by 1 |
| TC-INT-301 | Model updates on cardUpdated | Connect model → updateCard() | Model emits dataChanged for specific row |
| TC-INT-302 | Model updates on cardDeleted | Connect model → deleteCard() | Model emits rowsRemoved, rowCount decreases by 1 |
| TC-INT-303 | Model refresh on manual call | Load 3 cards → model.refresh() | Model reloads all 3 cards from manager |

### 7.2 Model Data Integrity

| ID | Scenario | Action | Expected Result |
|----|----------|--------|-----------------|
| TC-INT-310 | Model data matches database | Add 5 cards → Load model → Check data(index, NicknameRole) | All nicknames match database values |
| TC-INT-311 | Model displays masked card numbers | Add card with last4="1234" → model.data(index, MaskedNumberRole) | Returns "•••• •••• •••• 1234" |
| TC-INT-312 | Model shows expiry status | Add expired card → model.data(index, ExpiryStatusRole) | Returns "Expired" |

### 7.3 Model Filtering

| ID | Scenario | Action | Expected Result |
|----|----------|--------|-----------------|
| TC-INT-320 | Filter by search text | Add cards "My Visa", "Work Card" → setFilter("Visa") | Model shows only "My Visa" |
| TC-INT-321 | Filter returns no matches | Add cards → setFilter("NonExistent") | Model rowCount = 0 |
| TC-INT-322 | Clear filter | Apply filter → setFilter("") | Model shows all cards again |

---

## 8. Database Transaction Tests

### 8.1 Transaction Rollback

| ID | Scenario | Action | Expected Result |
|----|----------|--------|-----------------|
| TC-INT-350 | Rollback on batch import failure | Begin transaction → Import 5 cards → Fail on card 3 → Rollback | No cards saved (0-2 also rolled back) |
| TC-INT-351 | Commit on successful batch import | Begin transaction → Import 5 cards → All succeed → Commit | All 5 cards saved |

### 8.2 Concurrent Access

| ID | Scenario | Action | Expected Result |
|----|----------|--------|-----------------|
| TC-INT-360 | Read while write in progress | Thread A writes card → Thread B reads getAllCards() simultaneously | No deadlock, read returns consistent state |
| TC-INT-361 | Multiple writes sequentially | Add 10 cards rapidly in sequence | All cards saved correctly, no data corruption |

---

## 9. Cross-Component Integration Tests

### 9.1 Manager + Database + Encryption

| ID | Scenario | Action | Expected Result |
|----|----------|--------|-----------------|
| TC-INT-400 | End-to-end add card with all components | PaymentCardManager.addCard(card with CVC) → Uses CardEncryption → Saves to CardDatabase | Card saved with encrypted CVC, retrievable and decryptable |
| TC-INT-401 | End-to-end update card with CVC change | Add card with CVC="123" → Update CVC to "456" | Old CVC replaced, new CVC decryptable |
| TC-INT-402 | End-to-end delete with all components | Add card with CVC → Delete → Verify DB and manager state | Card and CVC completely removed |

### 9.2 Manager + Model Synchronization

| ID | Scenario | Action | Expected Result |
|----|----------|--------|-----------------|
| TC-INT-410 | Model stays in sync with manager | Connect model → Add card via manager | Model reflects new card immediately |
| TC-INT-411 | Model updates on manager signal | Update card nickname via manager → Model listens to cardUpdated | Model shows updated nickname |
| TC-INT-412 | Model handles multiple rapid changes | Add 10 cards rapidly → Model receives all signals | Model shows all 10 cards correctly |

---

## Test Environment Setup

### Required Components
- **Database**: Real SQLite database (not in-memory for integration tests)
- **Encryption**: Real CardEncryption with test keychain
- **Signals/Slots**: QSignalSpy for monitoring signals
- **Test Framework**: Qt Test with integration test fixtures

### Test Database Setup
```cpp
void IntegrationTest::initTestCase() {
    // Create temporary test database
    QString testDbPath = QDir::temp().filePath("test_cards.db");
    database = new CardDatabase();
    database->initialize(testDbPath);
    database->createTables();

    // Initialize encryption with test key
    encryption = CardEncryption::instance();
    encryption->initializeKey();

    // Create manager
    manager = PaymentCardManager::instance();
}

void IntegrationTest::cleanup() {
    // Clear all cards after each test
    auto cards = manager->getAllCards();
    for (const auto& card : cards) {
        manager->deleteCard(card.getId());
    }
}
```

### Signal Monitoring Example
```cpp
QSignalSpy addedSpy(manager, &PaymentCardManager::cardAdded);
manager->addCard(testCard);
QCOMPARE(addedSpy.count(), 1);
```

---

## Success Criteria

- **All integration tests pass**: 100% pass rate
- **Signal emission verified**: All state change signals emitted correctly
- **Database persistence verified**: All changes persisted across restarts
- **Encryption verified**: CVC encrypted at rest, decryptable after load
- **No data corruption**: Concurrent operations handled safely
- **Execution time**: All integration tests complete in < 30 seconds
